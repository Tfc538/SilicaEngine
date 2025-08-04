/**
 * @file Result.h
 * @brief Standardized error return value patterns for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Standard result types for consistent error handling across the engine.
 */

#pragma once

#include <string>
#include <optional>

namespace SilicaEngine {

    /// Standard result type for operations that can fail
    template<typename T>
    struct Result {
        bool success = false;
        std::string errorMessage;
        std::optional<T> value;
        
        static Result Success(const T& val) {
            Result result;
            result.success = true;
            result.value = val;
            return result;
        }
        
        static Result Error(const std::string& message) {
            Result result;
            result.success = false;
            result.errorMessage = message;
            return result;
        }
        
        // Implicit conversion to bool for easy checking
        operator bool() const { return success; }
        
        // Get value or default
        T GetValueOr(const T& defaultValue) const {
            return success && value.has_value() ? value.value() : defaultValue;
        }
    };
    
    /// Specialization for void results
    template<>
    struct Result<void> {
        bool success = false;
        std::string errorMessage;
        
        static Result Success() {
            Result result;
            result.success = true;
            return result;
        }
        
        static Result Error(const std::string& message) {
            Result result;
            result.success = false;
            result.errorMessage = message;
            return result;
        }
        
        // Implicit conversion to bool for easy checking
        operator bool() const { return success; }
    };
    
    /// Common result types
    using BoolResult = Result<bool>;
    using StringResult = Result<std::string>;
    using IntResult = Result<int>;
    
} // namespace SilicaEngine