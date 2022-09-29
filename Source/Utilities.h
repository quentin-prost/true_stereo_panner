/*
  ==============================================================================

    Utilities.h
    Created: 25 Sep 2022 10:20:46pm
    Author:  Quentin Prost

  ==============================================================================
*/

#pragma once

typedef enum {
    T1_64 = 0, T1_48, T1_32, T1_16, T1_12, T1_8, T1_6, T1_4,
    T1_3, T1_2, T1_1, T2_1, T3_1, T4_1, T6_1, T8_1, T16_1
} sync_rate_t;

inline void limitOutput(float* buffer, int sampleCount)
{
    if (buffer == nullptr) { return; }
    bool firstWarning = true;
    for (int i = 0; i < sampleCount; ++i) {
        float x = buffer[i];
        bool silence = false;
        if (std::isnan(x)) {
            DBG("!!! Nan detected in audio buffer, silencing !!!");
            silence = true;
        } else if (std::isinf(x)) {
            DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
            silence = true;
        } else if (x < -2.0f || x > 2.0f) {
            DBG("WARNING: sample out of range, silencing");
            silence = true;
        } else if (x < -1.0f) {
            if (firstWarning) {
                DBG("sample out of range, hard clipping");
                firstWarning = false;
            }
            buffer[i] = -1.0f;
        } else if (x > 1.0f) {
            if (firstWarning) {
                DBG("WARNING: sample out of range, hard clipping");
                firstWarning = false;
            }
            buffer[i] = 1.0f;
        }
        if (silence) {
            memset(buffer, 0, sampleCount * sizeof(float));
            return;
        }
    }
};

inline float get_rate_in_hz(sync_rate_t rate, float bpm) {
    float rate_hz;
    if (bpm <= __FLT_EPSILON__) jassertfalse;
    DBG(rate);
    //DBG(bpm);
    switch (rate) {
        case T1_64: rate_hz = 64.0f * bpm / 60.0f;
            break;
        case T1_48: rate_hz = 48.0f * bpm / 60.0f;
            break;
        case T1_32: rate_hz = 32.0f * bpm / 60.0f;
            break;
        case T1_16: rate_hz = 16.0f * bpm / 60.0f;
            break;
        case T1_12: rate_hz = 12.0f * bpm / 60.0f;
            break;
        case T1_8:  rate_hz = 8.0f * bpm / 60.0f;
            break;
        case T1_6:  rate_hz = 6.0f * bpm / 60.0f;
            break;
        case T1_4:  rate_hz = 4.0f * bpm / 60.0f;
            break;
        case T1_3:  rate_hz = 3.0f * bpm / 60.0f;
            break;
        case T1_2:  rate_hz = 2.0f * bpm / 60.0f;
            break;
        case T1_1:  rate_hz = 1.0f * bpm / 60.0f;
            break;
        case T2_1:  rate_hz = (1/2.0f) * bpm / 60.0f;
            break;
        case T3_1:  rate_hz = (1/3.0f) * bpm / 60.0f;
            break;
        case T4_1:  rate_hz = (1/4.0f) * bpm / 60.0f;
            break;
        case T8_1:  rate_hz = (1/8.0f) * bpm / 60.0f;
            break;
        case T16_1: rate_hz = (1/16.0f) * bpm / 60.0f;
            break;
        default: rate_hz = bpm / 60.0f;
            break;
    }
    return rate_hz;
}
