float current_value=0;
float alpha=0.3;
float previous_value;
//�˲�Ч��ȡ����a��Ȩ��
float EWMA_Filter(float current_value) {
    float current_filtered_value = alpha * current_value + (1 - alpha) * previous_value;
    previous_value = current_filtered_value; 
    return current_filtered_value;
}

float ADC_to_LightIntensity(float filtered_adc_value) {
    const float VREF = 3.3f;            // STM32的参考电压，单位为伏特
    const float PHOTO_RESISTANCE_DARK = 10000.0f; // 光敏电阻在黑暗中的阻值（单位：欧姆）
    const float PHOTO_RESISTANCE_LIGHT = 100.0f;  // 光敏电阻在强光中的阻值（单位：欧姆）
    const float MAX_LIGHT_INTENSITY = 100000.0f;  // 最大光照强度（单位：lux）

    // 将ADC值转换为电压
    float voltage = (filtered_adc_value / 4095.0f) * VREF;

    // 假设光敏电阻与一个固定电阻（如10kΩ）串联，计算光敏电阻的阻值
    float resistance = (VREF - voltage) / voltage * 10000.0f;

    // 将电阻值映射到光照强度
    if (resistance >= PHOTO_RESISTANCE_DARK) {
        return 0.0f; // 完全黑暗
    } else if (resistance <= PHOTO_RESISTANCE_LIGHT) {
        return MAX_LIGHT_INTENSITY; // 最大光照强度
    } else {
        // 线性映射
        return (PHOTO_RESISTANCE_DARK - resistance) / (PHOTO_RESISTANCE_DARK - PHOTO_RESISTANCE_LIGHT) * MAX_LIGHT_INTENSITY;
    }
	}

	