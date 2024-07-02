
void i2sInit() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX ),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S ),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
    .dma_buf_count = DMA_NUM_BUF,
    .dma_buf_len = DMA_BUF_LEN,
    .use_apll = true,
  };

  i2s_pin_config_t i2s_pin_config = {
    .bck_io_num = I2S_BCLK_PIN,
    .ws_io_num =  I2S_WCLK_PIN,
    .data_out_num = I2S_DOUT_PIN
  };

  i2s_driver_install(i2s_num, &i2s_config, 0, NULL);

  i2s_set_pin(i2s_num, &i2s_pin_config);
  i2s_zero_dma_buffer(i2s_num);
}


void i2sDeinit() {
  i2s_zero_dma_buffer(i2s_num);
  i2s_driver_uninstall(i2s_num);
}


inline void i2s_output () {
  // now out_buf is ready, output

    for (int i=0; i < DMA_BUF_LEN; i++) {      
    //  out_buf._signed[i*2] = 0x7fff * (float)(fast_tanh( mix_buf_l[i])) ; 
    //  out_buf._signed[i*2+1] = 0x7fff * (float)(fast_tanh( mix_buf_r[i])) ;
      out_buf._signed[i*2] = 0x7fff * (float)(( mix_buf_l[i])) ; 
      out_buf._signed[i*2+1] = 0x7fff * (float)(( mix_buf_r[i])) ;
    }
    i2s_write(i2s_num, out_buf._signed, sizeof(out_buf._signed), &bytes_written, portMAX_DELAY);

  xTaskNotifyGive(SynthTask1);
}
