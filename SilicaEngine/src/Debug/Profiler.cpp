/**
 * @file Profiler.cpp
 * @brief Implementation of performance profiling system
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Debug/Profiler.h"
#include "SilicaEngine/Core/Logger.h"

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <psapi.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <fstream>
#endif

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <once.h>

namespace SilicaEngine {

    // === GPUTimer Implementation ===
    
    GPUTimer::GPUTimer() {
        glGenQueries(2, m_queryIDs);
    }
    
    GPUTimer::~GPUTimer() {
        if (m_queryIDs[0] != 0) {
            glDeleteQueries(2, m_queryIDs);
        }
    }
    
    void GPUTimer::Begin() {
        if (m_queryIDs[0] == 0) return;
        
        glQueryCounter(m_queryIDs[0], GL_TIMESTAMP);
        m_active = true;
    }
    
    void GPUTimer::End() {
        if (m_queryIDs[1] == 0 || !m_active) return;
        
        glQueryCounter(m_queryIDs[1], GL_TIMESTAMP);
    }
    
    bool GPUTimer::IsReady() const {
        if (!m_active || m_queryIDs[1] == 0) return false;
        
        GLint available = 0;
        glGetQueryObjectiv(m_queryIDs[1], GL_QUERY_RESULT_AVAILABLE, &available);
        return available == GL_TRUE;
    }
    
    double GPUTimer::GetTimeMs() const {
        if (!IsReady()) return 0.0;
        
        GLuint64 startTime, endTime;
        glGetQueryObjectui64v(m_queryIDs[0], GL_QUERY_RESULT, &startTime);
        glGetQueryObjectui64v(m_queryIDs[1], GL_QUERY_RESULT, &endTime);
        
        // Convert nanoseconds to milliseconds
        return (endTime - startTime) / 1000000.0;
    }

    // === ProfileScope Implementation ===
    
    ProfileScope::ProfileScope(const std::string& name, bool enableGPU) 
        : m_name(name), m_gpuEnabled(enableGPU) {
        
        if (!Profiler::IsEnabled()) return;
        
        m_cpuTimer.Begin();
        
        if (m_gpuEnabled) {
            m_gpuTimer = std::make_unique<GPUTimer>();
            m_gpuTimer->Begin();
        }
        
        Profiler::BeginScope(name, enableGPU);
    }
    
    ProfileScope::~ProfileScope() {
        if (!Profiler::IsEnabled()) return;
        
        m_cpuTimer.End();
        
        if (m_gpuTimer) {
            m_gpuTimer->End();
        }
        
        double cpuTime = m_cpuTimer.GetTimeMs();
        double gpuTime = 0.0;
        
        if (m_gpuTimer && m_gpuTimer->IsReady()) {
            gpuTime = m_gpuTimer->GetTimeMs();
        }
        
        Profiler::AddSample(m_name, cpuTime, gpuTime);
        Profiler::EndScope();
    }

    // === Profiler Implementation ===
    
    Profiler& Profiler::GetInstance() {
        static std::once_flag flag;
        static Profiler* instance = nullptr;
        std::call_once(flag, []() {
            instance = new Profiler();
        });
        return *instance;
    }
    
    bool Profiler::Initialize() {
        auto& instance = GetInstance();
        
        if (instance.m_initialized) {
            SE_WARN("Profiler already initialized");
            return true;
        }
        
        SE_INFO("Initializing Profiler");
        
        // Check for GPU timer support
        GLint available;
        glGetQueryiv(GL_TIMESTAMP, GL_QUERY_COUNTER_BITS, &available);
        
        if (available == 0) {
            SE_WARN("GPU timing not supported on this hardware");
        } else {
            SE_INFO("GPU timing supported ({} bits precision)", available);
        }
        
        instance.m_initialized = true;
        instance.m_frameStartTime = std::chrono::high_resolution_clock::now();
        
        SE_INFO("Profiler initialized successfully");
        return true;
    }
    
    void Profiler::Shutdown() {
        auto& instance = GetInstance();
        
        if (!instance.m_initialized) return;
        
        SE_INFO("Shutting down Profiler");
        
        ClearResults();
        instance.m_activeGPUTimers.clear();
        instance.m_initialized = false;
        
        SE_INFO("Profiler shutdown complete");
    }
    
    bool Profiler::IsEnabled() {
        auto& instance = GetInstance();
        return instance.m_enabled && instance.m_initialized;
    }
    
    void Profiler::SetEnabled(bool enabled) {
        auto& instance = GetInstance();
        instance.m_enabled = enabled;
    }
    
    void Profiler::BeginScope(const std::string& name, bool enableGPU) {
        auto& instance = GetInstance();
        instance.BeginScopeInternal(name, Category::Custom, enableGPU);
    }
    
    void Profiler::EndScope() {
        auto& instance = GetInstance();
        instance.EndScopeInternal();
    }
    
    void Profiler::AddSample(const std::string& name, double cpuTimeMs, double gpuTimeMs) {
        auto& instance = GetInstance();
        instance.AddSampleInternal(name, cpuTimeMs, gpuTimeMs, Category::Custom);
    }
    
    std::vector<ProfileSample> Profiler::GetResults() {
        auto& instance = GetInstance();
        std::lock_guard<std::mutex> lock(instance.m_profilerMutex);
        
        std::vector<ProfileSample> results;
        results.reserve(instance.m_samples.size());
        
        for (const auto& [name, sample] : instance.m_samples) {
            results.push_back(sample);
        }
        
        // Sort by average time (descending)
        std::sort(results.begin(), results.end(), 
                  [](const ProfileSample& a, const ProfileSample& b) {
                      return a.avgTimeMs > b.avgTimeMs;
                  });
        
        return results;
    }
    
    ProfileSample Profiler::GetSample(const std::string& name) {
        auto& instance = GetInstance();
        std::lock_guard<std::mutex> lock(instance.m_profilerMutex);
        
        auto it = instance.m_samples.find(name);
        if (it != instance.m_samples.end()) {
            return it->second;
        }
        
        return ProfileSample{}; // Return empty sample if not found
    }
    
    void Profiler::ClearResults() {
        auto& instance = GetInstance();
        std::lock_guard<std::mutex> lock(instance.m_profilerMutex);
        
        instance.m_samples.clear();
        instance.m_sampleCategories.clear();
        instance.m_frameTimes.clear();
        
        // Clear scope stack
        while (!instance.m_scopeStack.empty()) {
            instance.m_scopeStack.pop();
        }
    }
    
    void Profiler::BeginFrame() {
        auto& instance = GetInstance();
        if (!IsEnabled()) return;
        
        instance.m_frameStartTime = std::chrono::high_resolution_clock::now();
        instance.UpdateGPUTimers();
        instance.UpdateMemoryUsage();
    }
    
    void Profiler::EndFrame() {
        auto& instance = GetInstance();
        if (!IsEnabled()) return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = endTime - instance.m_frameStartTime;
        double frameTimeMs = std::chrono::duration<double, std::milli>(duration).count();
        
        std::lock_guard<std::mutex> lock(instance.m_profilerMutex);
        
        instance.m_frameTimes.push_back(frameTimeMs);
        
        // Keep only the last N frame times
        if (instance.m_frameTimes.size() > instance.m_maxFrameTimes) {
            instance.m_frameTimes.erase(instance.m_frameTimes.begin());
        }
    }
    
    double Profiler::GetAverageFPS(int frameCount) {
        auto& instance = GetInstance();
        std::lock_guard<std::mutex> lock(instance.m_profilerMutex);
        
        if (instance.m_frameTimes.empty()) return 0.0;
        
        size_t sampleCount = std::min(static_cast<size_t>(frameCount), instance.m_frameTimes.size());
        
        double totalTime = 0.0;
        for (size_t i = instance.m_frameTimes.size() - sampleCount; i < instance.m_frameTimes.size(); ++i) {
            totalTime += instance.m_frameTimes[i];
        }
        
        double avgFrameTime = totalTime / sampleCount;
        return avgFrameTime > 0.0 ? 1000.0 / avgFrameTime : 0.0;
    }
    
    double Profiler::GetFrameTimeMs() {
        auto& instance = GetInstance();
        std::lock_guard<std::mutex> lock(instance.m_profilerMutex);
        
        return instance.m_frameTimes.empty() ? 0.0 : instance.m_frameTimes.back();
    }
    
    size_t Profiler::GetGPUMemoryUsage() {
        // This is vendor-specific and may not be available on all systems
        GLint totalMemory = 0;
        GLint availableMemory = 0;
        
        // NVIDIA extension
        if (glfwExtensionSupported("GL_NVX_gpu_memory_info")) {
            glGetIntegerv(0x9048, &totalMemory);     // GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
            glGetIntegerv(0x9049, &availableMemory); // GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX
            return static_cast<size_t>(totalMemory - availableMemory) * 1024; // Convert KB to bytes
        }
        
        // AMD extension
        if (glfwExtensionSupported("GL_ATI_meminfo")) {
            GLint param[4];
            glGetIntegerv(0x87FC, param); // GL_TEXTURE_FREE_MEMORY_ATI
            return static_cast<size_t>(param[0]) * 1024; // Convert KB to bytes
        }
        
        return 0; // Unknown
    }
    
    size_t Profiler::GetCPUMemoryUsage() {
#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize;
        }
#elif defined(__linux__)
        std::ifstream file("/proc/self/status");
        std::string line;
        while (std::getline(file, line)) {
            if (line.substr(0, 6) == "VmRSS:") {
                std::istringstream iss(line);
                std::string key, value, unit;
                iss >> key >> value >> unit;
                return std::stoul(value) * 1024; // Convert KB to bytes
            }
        }
#endif
        return 0;
    }
    
    // Helper function to escape JSON special characters
    std::string EscapeJsonString(const std::string& str) {
        std::string escaped;
        escaped.reserve(str.length() + 10); // Reserve some extra space for escape sequences
        
        for (char c : str) {
            switch (c) {
                case '"':  escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default:
                    if (c < 0x20) {
                        // Escape other control characters
                        char buf[7];
                        std::sprintf(buf, "\\u%04x", static_cast<unsigned char>(c));
                        escaped += buf;
                    } else {
                        escaped += c;
                    }
                    break;
            }
        }
        return escaped;
    }

    std::string Profiler::ExportToJSON() {
        auto results = GetResults();
        
        std::ostringstream json;
        json << "{\n  \"profiling_data\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& sample = results[i];
            json << "    {\n";
            json << "      \"name\": \"" << EscapeJsonString(sample.name) << "\",\n";
            json << "      \"cpu_time_ms\": " << sample.cpuTimeMs << ",\n";
            json << "      \"gpu_time_ms\": " << sample.gpuTimeMs << ",\n";
            json << "      \"call_count\": " << sample.callCount << ",\n";
            json << "      \"min_time_ms\": " << sample.minTimeMs << ",\n";
            json << "      \"max_time_ms\": " << sample.maxTimeMs << ",\n";
            json << "      \"avg_time_ms\": " << sample.avgTimeMs << "\n";
            json << "    }";
            
            if (i < results.size() - 1) {
                json << ",";
            }
            json << "\n";
        }
        
        json << "  ],\n";
        json << "  \"fps\": " << GetAverageFPS() << ",\n";
        json << "  \"frame_time_ms\": " << GetFrameTimeMs() << "\n";
        json << "}";
        
        return json.str();
    }
    
    // === Private Implementation ===
    
    void Profiler::BeginScopeInternal(const std::string& name, Category category, [[maybe_unused]] bool enableGPU) {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(m_profilerMutex);
        
        // Set up hierarchical relationship
        if (!m_scopeStack.empty()) {
            const std::string& parentName = m_scopeStack.top();
            m_samples[name].parentName = parentName;
            m_samples[parentName].children.push_back(name);
        }
        
        m_scopeStack.push(name);
        m_samples[name].depth = static_cast<int>(m_scopeStack.size()) - 1;
        m_sampleCategories[name] = category;
    }
    
    void Profiler::EndScopeInternal() {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(m_profilerMutex);
        
        if (!m_scopeStack.empty()) {
            m_scopeStack.pop();
        }
    }
    
    void Profiler::AddSampleInternal(const std::string& name, double cpuTimeMs, double gpuTimeMs, Category category) {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(m_profilerMutex);
        
        auto& sample = m_samples[name];
        sample.name = name;
        sample.cpuTimeMs = cpuTimeMs;
        sample.gpuTimeMs = gpuTimeMs;
        sample.AddSample(cpuTimeMs + gpuTimeMs);
        
        m_sampleCategories[name] = category;
        
        // Limit number of samples
        if (m_samples.size() > m_maxSamples) {
            // Remove oldest samples (simplified - could be more sophisticated)
            auto it = m_samples.begin();
            m_samples.erase(it);
        }
    }
    
    void Profiler::UpdateGPUTimers() {
        // Update any active GPU timers
        auto it = m_activeGPUTimers.begin();
        while (it != m_activeGPUTimers.end()) {
            if ((*it)->IsReady()) {
                it = m_activeGPUTimers.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void Profiler::UpdateMemoryUsage() {
        m_currentCPUMemory = GetCPUMemoryUsage();
        m_peakCPUMemory = std::max(m_peakCPUMemory, m_currentCPUMemory);
    }

} // namespace SilicaEngine