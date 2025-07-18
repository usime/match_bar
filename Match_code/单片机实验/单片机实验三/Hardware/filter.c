float current_value=0;
float alpha=0.3;
float previous_value;
//滤波效果取决于a的权重
float EWMA_Filter(float current_value) {
    float current_filtered_value = alpha * current_value + (1 - alpha) * previous_value;
    previous_value = current_filtered_value; 
    return current_filtered_value;
}

float ADC_to_LightIntensity(float filtered_adc_value) {
    const float VREF = 3.3f;            // STM32鐨勫弬鑰冪數鍘嬶紝鍗曚綅涓轰紡鐗�
    const float PHOTO_RESISTANCE_DARK = 10000.0f; // 鍏夋晱鐢甸樆鍦ㄩ粦鏆椾腑鐨勯樆鍊硷紙鍗曚綅锛氭濮嗭級
    const float PHOTO_RESISTANCE_LIGHT = 100.0f;  // 鍏夋晱鐢甸樆鍦ㄥ己鍏変腑鐨勯樆鍊硷紙鍗曚綅锛氭濮嗭級
    const float MAX_LIGHT_INTENSITY = 100000.0f;  // 鏈�澶у厜鐓у己搴︼紙鍗曚綅锛歭ux锛�

    // 灏咥DC鍊艰浆鎹负鐢靛帇
    float voltage = (filtered_adc_value / 4095.0f) * VREF;

    // 鍋囪鍏夋晱鐢甸樆涓庝竴涓浐瀹氱數闃伙紙濡�10k惟锛変覆鑱旓紝璁＄畻鍏夋晱鐢甸樆鐨勯樆鍊�
    float resistance = (VREF - voltage) / voltage * 10000.0f;

    // 灏嗙數闃诲�兼槧灏勫埌鍏夌収寮哄害
    if (resistance >= PHOTO_RESISTANCE_DARK) {
        return 0.0f; // 瀹屽叏榛戞殫
    } else if (resistance <= PHOTO_RESISTANCE_LIGHT) {
        return MAX_LIGHT_INTENSITY; // 鏈�澶у厜鐓у己搴�
    } else {
        // 绾挎�ф槧灏�
        return (PHOTO_RESISTANCE_DARK - resistance) / (PHOTO_RESISTANCE_DARK - PHOTO_RESISTANCE_LIGHT) * MAX_LIGHT_INTENSITY;
    }
	}

	