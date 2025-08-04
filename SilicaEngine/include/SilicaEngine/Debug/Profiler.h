/**
 * @file Profiler.h
 * @brief Performance profiling system with GPU/CPU timing
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Comprehensive profiling system for tracking CPU and GPU performance.
 */

#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <stack>

namespace SilicaEngine {

    /// Profiling sample data
    struct ProfileSample {
        std::string name;
        double cpuTimeMs = 0.0;
        double gpuTimeMs = 0.0;
        uint64_t callCount = 0;
        double minTimeMs = std::numeric_limits<double>::max();
        double maxTimeMs = 0.0;
        double avgTimeMs = 0.0;
        
        // Hierarchical profiling
        std::string parentName;
        std::vector<std::string> children;
        int depth = 0;
        
        void AddSample(double timeMs) {
            callCount++;
            minTimeMs = std::min(minTimeMs, timeMs);
            maxTimeMs = std::max(maxTimeMs, timeMs);
            avgTimeMs = ((avgTimeMs * (callCount - 1)) + timeMs) / callCount;
        }
    };

    /// GPU timer query wrapper
    class GPUTimer {
    public:
        GPUTimer();
        ~GPUTimer();
        
        void Begin();
        void End();
        bool IsReady() const;
        double GetTimeMs() const;
        
    private:
        uint32_t m_queryIDs[2] = {0, 0};  // Begin and end queries
        bool m_active = false;
    };

    /// CPU timer using high resolution clock
    class CPUTimer {
    public:
        CPUTimer() = default;
        
        void Begin() {
            m_startTime = std::chrono::high_resolution_clock::now();
        }
        
        void End() {
            m_endTime = std::chrono::high_resolution_clock::now();
        }
        
        double GetTimeMs() const {
            auto duration = m_endTime - m_startTime;
            return std::chrono::duration<double, std::milli>(duration).count();
        }
        
    private:
        std::chrono::high_resolution_clock::time_point m_startTime;
        std::chrono::high_resolution_clock::time_point m_endTime;
    };

    /// Profiling scope for automatic timing
    class ProfileScope {
    public:
        ProfileScope(const std::string& name, bool enableGPU = true);
        ~ProfileScope();
        
    private:
        std::string m_name;
        CPUTimer m_cpuTimer;
        std::unique_ptr<GPUTimer> m_gpuTimer;
        bool m_gpuEnabled;
    };

    /// Main profiler class
    class Profiler {
    public:
        static Profiler& GetInstance();
        
        /// Initialize the profiler
        static bool Initialize();
        
        /// Shutdown the profiler
        static void Shutdown();
        
        /// Check if profiler is enabled
        static bool IsEnabled();
        
        /// Enable/disable profiling globally
        static void SetEnabled(bool enabled);
        
        /// Begin a profiling scope
        static void BeginScope(const std::string& name, bool enableGPU = true);
        
        /// End the current profiling scope
        static void EndScope();
        
        /// Add a manual sample
        static void AddSample(const std::string& name, double cpuTimeMs, double gpuTimeMs = 0.0);
        
        /// Get profiling results
        static std::vector<ProfileSample> GetResults();
        
        /// Get results for a specific sample
        static ProfileSample GetSample(const std::string& name);
        
        /// Clear all profiling data
        static void ClearResults();
        
        /// Set maximum number of samples to keep
        static void SetMaxSamples(size_t maxSamples);
        
        /// Export results to JSON format
        static std::string ExportToJSON();
        
        /// Export results to CSV format
        static std::string ExportToCSV();
        
        /// Get current frame profiling data
        static std::vector<ProfileSample> GetFrameData();
        
        /// Start a new frame (resets frame-specific data)
        static void BeginFrame();
        
        /// End the current frame
        static void EndFrame();
        
        /// Get average FPS over the last N frames
        static double GetAverageFPS(int frameCount = 60);
        
        /// Get current frame time in milliseconds
        static double GetFrameTimeMs();
        
        /// Get GPU memory usage (if available)
        static size_t GetGPUMemoryUsage();
        
        /// Get CPU memory usage
        static size_t GetCPUMemoryUsage();
        
        /// Set profiling categories
        enum class Category {
            Rendering,
            Physics,
            Audio,
            Input,
            AssetLoading,
            Scripting,
            UI,
            Custom
        };
        
        /// Begin scope with category
        static void BeginScopeWithCategory(const std::string& name, Category category, bool enableGPU = true);
        
        /// Get results by category
        static std::vector<ProfileSample> GetResultsByCategory(Category category);

    private:
        Profiler() = default;
        ~Profiler() = default;
        
        /// Internal sample storage
        std::unordered_map<std::string, ProfileSample> m_samples;
        std::unordered_map<std::string, Category> m_sampleCategories;
        
        /// Hierarchical profiling stack
        std::stack<std::string> m_scopeStack;
        
        /// Frame timing
        std::vector<double> m_frameTimes;
        std::chrono::high_resolution_clock::time_point m_frameStartTime;
        size_t m_maxFrameTimes = 120; // Keep 2 seconds worth at 60fps
        
        /// Settings
        bool m_enabled = true;
        bool m_initialized = false;
        size_t m_maxSamples = 1000;
        
        /// Thread safety
        std::mutex m_profilerMutex;
        
        /// Internal methods
        void BeginScopeInternal(const std::string& name, Category category, bool enableGPU);
        void EndScopeInternal();
        void AddSampleInternal(const std::string& name, double cpuTimeMs, double gpuTimeMs, Category category);
        
        /// GPU query management
        std::vector<std::unique_ptr<GPUTimer>> m_activeGPUTimers;
        void UpdateGPUTimers();
        
        /// Memory tracking
        size_t m_peakCPUMemory = 0;
        size_t m_currentCPUMemory = 0;
        void UpdateMemoryUsage();
    };

    /// Convenience macros for profiling
    #ifdef SILICA_PROFILE
        #define SE_PROFILE_SCOPE(name) \
            SilicaEngine::ProfileScope _profile_scope(name)
        
        #define SE_PROFILE_SCOPE_GPU(name) \
            SilicaEngine::ProfileScope _profile_scope(name, true)
        
        #define SE_PROFILE_SCOPE_CPU(name) \
            SilicaEngine::ProfileScope _profile_scope(name, false)
        
        #define SE_PROFILE_FUNCTION() \
            SE_PROFILE_SCOPE(__FUNCTION__)
        
        #define SE_PROFILE_CATEGORY(name, category) \
            SilicaEngine::Profiler::BeginScopeWithCategory(name, category); \
            auto _profile_guard = SilicaEngine::make_scope_exit([]() { SilicaEngine::Profiler::EndScope(); })
        
        #define SE_PROFILE_BEGIN_FRAME() \
            SilicaEngine::Profiler::BeginFrame()
        
        #define SE_PROFILE_END_FRAME() \
            SilicaEngine::Profiler::EndFrame()
        
        #define SE_PROFILE_MANUAL(name, timeMs) \
            SilicaEngine::Profiler::AddSample(name, timeMs)
            
    #else
        #define SE_PROFILE_SCOPE(name) do {} while(0)
        #define SE_PROFILE_SCOPE_GPU(name) do {} while(0)
        #define SE_PROFILE_SCOPE_CPU(name) do {} while(0)
        #define SE_PROFILE_FUNCTION() do {} while(0)
        #define SE_PROFILE_CATEGORY(name, category) do {} while(0)
        #define SE_PROFILE_BEGIN_FRAME() do {} while(0)
        #define SE_PROFILE_END_FRAME() do {} while(0)
        #define SE_PROFILE_MANUAL(name, timeMs) do {} while(0)
    #endif

    /// Utility for scope guards
    template<typename F>
    struct ScopeExit {
        F f;
        ScopeExit(F&& f) : f(std::forward<F>(f)) {}
        ~ScopeExit() { f(); }
    };
    
    template<typename F>
    ScopeExit<F> make_scope_exit(F&& f) {
        return ScopeExit<F>(std::forward<F>(f));
    }

} // namespace SilicaEngine