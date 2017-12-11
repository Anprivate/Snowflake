#include <Adafruit_NeoPixel.h>

// Пин на который цепляются светодиоды
#define PIN 3

// Число лучей
#define ray_number 6

// Число пикселей в луче
#define ray_pixel 10

// всего пикселей
#define total_pixels (ray_number * ray_pixel)

// Фактор оверсемпла
#define oversample  4

// Длительность кадра
#define frame_period 10

Adafruit_NeoPixel strip = Adafruit_NeoPixel(total_pixels, PIN, NEO_GRB + NEO_KHZ800);

// яркость
uint8_t cur_br = 25;

// буфер для передискретизации
#define os_buffer_length ((ray_pixel + 2) * oversample)
uint32_t os_buffer[os_buffer_length];

// буфер для одного луча
uint32_t one_ray[ray_pixel];

void setup() {
  strip.begin();
  strip.show();
  SetBrightness();
}

void loop() {
  static uint8_t prev_mode = 0;

  uint8_t curr_mode;
  curr_mode = random(6);
  if (curr_mode >= prev_mode) curr_mode++;
  prev_mode = curr_mode;

  switch (curr_mode)
    //  switch (3)
  {
    case 0: {
        ShowMode2();
        break;
      }
    case 1: {
        ShowMode3();
        break;
      }
    case 2: {
        ShowMode4();
        break;
      }
    case 3: {
        ShowMode5();
        break;
      }
    default:
      ShowMode1();
  }
  delay(300);
}

void ShowMode1(void) {
  uint8_t col1_mask = random(6) + 1;
  uint8_t col2_mask = random(5) + 1;
  if (col2_mask >= col1_mask) col2_mask++;

  uint32_t color1 = mask_to_color(col1_mask);
  uint32_t color2 = mask_to_color(col2_mask);

  create_os_buffer(color1, color2);

  // задаём режим выдачи лучей
  uint8_t ray_mode1 = random(6);
  // оффсет для некоторых режимов
  uint8_t mode_offset = random(6);

  delay_sp(0);
  for (int16_t start_idx = -(ray_pixel * oversample); start_idx <= (ray_pixel * (oversample + 1)); start_idx++) {
    SetBrightness();
    clear_all();

    // берём луч из буфера со смещением
    os_buffer_to_ray(start_idx);

    // выдаём готовый луч в нужные лучи
    switch (ray_mode1) {
      case 0: { // изнутри наружу
          for (uint8_t i = 0; i < ray_number; i++) buffer_to_ray(i, 1);
          break;
        }
      case 1: { // снаружи внутрь
          for (uint8_t i = 0; i < ray_number; i++) buffer_to_ray(i, 0);
          break;
        }
      case 2: { // через один
          for (uint8_t i = 0; i < ray_number; i++) buffer_to_ray(i, i % 2);
          break;
        }
      case 3: { // через один наоборот
          for (uint8_t i = 0; i < ray_number; i++) buffer_to_ray(i, (i + 1) % 2);
          break;
        }
      case 4:
      case 5: { // пополам со случайным оффсетом
          for (uint8_t i = 0; i < ray_number; i++) buffer_to_ray(i, ((i + mode_offset) / 3) % 2);
          break;
        }
    }

    strip.show();
    delay_sp(frame_period);
  }
}

void ShowMode2(void) {
  uint8_t col1_mask = random(6) + 1;
  uint8_t col2_mask = random(5) + 1;
  if (col2_mask >= col1_mask) col2_mask++;

  uint32_t color1 = mask_to_color(col1_mask);
  uint32_t color2 = mask_to_color(col2_mask);

  create_os_buffer(color1, color2);

  // оффсет для лучей
  uint8_t mode_offset = random(6);

  delay_sp(0);
  for (int16_t start_idx = -(ray_pixel * oversample); start_idx <= oversample; start_idx++) {
    SetBrightness();
    clear_all();

    // берём луч из буфера со смещением
    os_buffer_to_ray(start_idx);

    // выдаём готовый луч в нужные лучи
    uint8_t curr_ray = mode_offset % (ray_number / 2);
    buffer_to_ray(curr_ray, 1);
    curr_ray += ray_number / 2;
    buffer_to_ray(curr_ray, 1);

    strip.show();
    delay_sp(frame_period);
  }

  // крутим луч вокруг оси
  uint8_t curr_ray = mode_offset % (ray_number / 2);
  for (uint8_t i = 1; i <= ray_number; i++) {
    delay(300);
    SetBrightness();
    clear_all();
    curr_ray++;
    buffer_to_ray(curr_ray % ray_number, 1);
    buffer_to_ray((curr_ray + ray_number / 2) % ray_number, 1);
    strip.show();
  }

  delay(300);
  delay_sp(0);

  for (int16_t start_idx = oversample; start_idx <= (ray_pixel * (oversample + 1)); start_idx++) {
    SetBrightness();
    clear_all();

    // берём луч из буфера со смещением
    os_buffer_to_ray(start_idx);

    // выдаём готовый луч в нужные лучи
    uint8_t curr_ray = mode_offset % (ray_number / 2);
    buffer_to_ray(curr_ray, 1);
    curr_ray += ray_number / 2;
    buffer_to_ray(curr_ray, 1);

    strip.show();
    delay_sp(frame_period);
  }
}

void ShowMode3(void) {
  uint8_t col1_mask = random(6) + 1;
  uint8_t col2_mask = random(5) + 1;
  if (col2_mask >= col1_mask) col2_mask++;

  uint32_t color1 = mask_to_color(col1_mask);
  uint32_t color2 = mask_to_color(col2_mask);

  create_os_buffer(color1, color2);

  int16_t factor = 5 + random(5);
  int8_t ray_off = random(ray_number);
  uint8_t ray_dir = random(2);
  uint8_t center_dir = random(2);

  delay_sp(0);
  for (int16_t start_idx = -(ray_pixel * oversample); start_idx <= (ray_pixel * (oversample + 1) + ray_number * factor); start_idx++) {
    SetBrightness();
    clear_all();
    for (uint8_t ray_num = 0; ray_num < ray_number; ray_num++) {
      // берём луч из буфера со смещением
      int16_t offset;
      if (ray_dir)
        offset = (ray_num + ray_off) % ray_number * factor;
      else
        offset = (ray_number - ray_num - 1 + ray_off) % ray_number * factor;
      os_buffer_to_ray(start_idx - offset);

      // выдаём готовый луч
      buffer_to_ray(ray_num, center_dir);
    }
    strip.show();
    delay_sp(frame_period);
  }
}

void ShowMode4(void) {
  // число звёзд
#define star_number (total_pixels * 2 / 3)
  // всего 5000 миллисекунд
#define total_cycles (5000 / frame_period)
  // разгорание за полсекунды
#define speed_cntr (500 / frame_period)

  uint8_t predelay[star_number];
  uint32_t color[star_number];
  uint8_t pixel_num[star_number];

  for (uint8_t i = 0; i < star_number; i++) {
    // первичная задержка до 1 секунды
    predelay[i] = random(100);
    color[i] = 0;
    pixel_num[i] = 0xFF;
  }

  delay_sp(0);
  for (uint16_t time_counter = 0; time_counter < total_cycles; time_counter++) {
    SetBrightness();
    clear_all();

    for (uint8_t curr_star = 0; curr_star < star_number; curr_star++) {
      if (color[curr_star] == 0) { // predelay stage
        if (predelay[curr_star])
          predelay[curr_star]--;
        else
          // predelay закончился, задаём любой цвет
          color[curr_star] = 1;
      } else {
        if (predelay[curr_star] == 0) { // начальная фаза, генерим параметры пикселя
          // ищем свободный пиксель
          uint8_t tmp_num;
          uint8_t busy;
          do {
            tmp_num = random(ray_number * ray_pixel);
            busy = 0;
            for (uint8_t i = 0; i < star_number; i++) {
              if (pixel_num[i] == tmp_num) {
                busy = 1;
                break;
              }
            }
          } while (busy);
          pixel_num[curr_star] = tmp_num;
          color[curr_star] = mask_to_color(random(6) + 1);
          predelay[curr_star]++;
        } else {
          // рабочая фаза, старший бит обозначает направление
          if (predelay[curr_star] & 0x80) { // fade down
            strip.setPixelColor(pixel_num[curr_star], mult_div_color(color[curr_star], predelay[curr_star] & 0x7F, speed_cntr));
            predelay[curr_star]--;
            if ((predelay[curr_star] & 0x7F) == 0) predelay[curr_star] = 0;
          } else { // fade up
            strip.setPixelColor(pixel_num[curr_star], mult_div_color(color[curr_star], predelay[curr_star], speed_cntr));
            predelay[curr_star]++;
            if (predelay[curr_star] >= speed_cntr) predelay[curr_star] |= 0x80;
          }
        }
      }
    }
    strip.show();
    delay_sp(frame_period);
  }
  // финал - плавненько гасим все пиксели
  uint8_t some_action;
  do {
    SetBrightness();
    clear_all();
    some_action = 0;
    for (uint8_t curr_star = 0; curr_star < star_number; curr_star++) {
      predelay[curr_star] &= 0x7F;
      strip.setPixelColor(pixel_num[curr_star], mult_div_color(color[curr_star], predelay[curr_star], speed_cntr));
      if (predelay[curr_star]) {
        predelay[curr_star]--;
        some_action = 1;
      }
    }
    strip.show();
    delay_sp(frame_period);
  } while (some_action);
}

void ShowMode5(void) {
#define gap 0
  uint8_t col1_mask = random(6) + 1;
  uint8_t col2_mask = random(5) + 1;
  if (col2_mask >= col1_mask) col2_mask++;

  uint32_t color1 = mask_to_color(col1_mask);
  uint32_t color2 = mask_to_color(col2_mask);

  create_os_buffer(color1, color2);

  delay_sp(0);
  uint8_t dir = random(2);
  for (int16_t start_idx = -(ray_pixel * oversample); start_idx <= (2 * ray_pixel + gap) * (oversample + 1); start_idx++) {
    SetBrightness();
    clear_all();

    // берём луч из буфера со смещением
    os_buffer_to_ray(start_idx);

    // выдаём готовый луч
    for (uint8_t i = 0; i < ray_number; i += 2) buffer_to_ray((i + dir) % ray_number, 1);

    // берём луч из буфера со смещением
    os_buffer_to_ray(start_idx - (ray_pixel + gap) * oversample);

    // выдаём готовый луч
    for (uint8_t i = 1; i < ray_number; i += 2) buffer_to_ray((i + dir) % ray_number, 0);

    strip.show();
    delay_sp(frame_period);
  }
}

// ================================================================
// вспомогательные функции
// ================================================================

// задержка на заданное число миллисекунд от последнего вызова (0 - выйти сразу, но зафиксировать вызов)
void delay_sp(uint16_t value) {
  static uint32_t last_call_time;
  if (value) {
    uint32_t between = millis() - last_call_time;
    if ((between < 10000) && (between < value)) delay(value - between);
  }
  last_call_time = millis();
}

// копируем в буфер луча кусок сглаженного буфера с шагом oversample и заданным смещением
void os_buffer_to_ray(int16_t curr_idx) {
  for (uint8_t i = 0; i < ray_pixel; i++) {
    int16_t os_buff_idx = curr_idx + i * oversample;
    if (os_buff_idx_is_valid(os_buff_idx))
      one_ray[i] = os_buffer[os_buff_idx];
    else
      one_ray[i] = 0;
  }
}

// номер пикселя в луче допустим?
inline uint8_t pixel_num_is_valid(int16_t pixel_num) {
  return (pixel_num >= 0) && (pixel_num < ray_pixel);
}

// индекс элемента в массиве допустим?
inline uint8_t os_buff_idx_is_valid(int16_t idx) {
  return (idx >= 0) && (idx < os_buffer_length);
}

// маска цветов в цвет
uint32_t mask_to_color(uint8_t mask) {
  uint32_t tmp_color = 0;
  uint32_t maskbit = 0x000000FF;
  if (mask & 0x01) tmp_color += maskbit;
  maskbit <<= 8;
  if (mask & 0x02) tmp_color += maskbit;
  (maskbit <<= 8);
  if (mask & 0x04) tmp_color += maskbit;
  return tmp_color;
}

// берём RGB цвета, умножаем на mult и делим на divider
uint32_t mult_div_color(uint32_t in_color, uint16_t mult, uint16_t divider) {
  uint32_t rn = ((in_color >> 16) & 0xFF) * mult / divider;
  uint32_t gn = ((in_color >> 8) & 0xFF) * mult / divider;
  uint32_t bn = (in_color & 0xFF) * mult / divider;
  return (rn << 16) | (gn << 8) | bn;
}

// создаёт сглаженный буфер с в oversample раз больше, чем длина луча
void create_os_buffer(uint32_t color1, uint32_t color2) {
  int16_t r1 = (color1 >> 16) & 0xFF;
  int16_t r2 = (color2 >> 16) & 0xFF;
  int16_t g1 = (color1 >> 8) & 0xFF;
  int16_t g2 = (color2 >> 8) & 0xFF;
  int16_t b1 = color1 & 0xFF;
  int16_t b2 = color2 & 0xFF;

  int16_t total_steps = ray_pixel * oversample;
  // разгорание
  for (uint8_t step_num = 0; step_num < oversample; step_num++) {
    uint8_t rn = (r1 * step_num) / oversample;
    uint8_t gn = (g1 * step_num) / oversample;
    uint8_t bn = (b1 * step_num) / oversample;
    os_buffer[step_num] = bn + ( (uint32_t) gn << 8) + ((uint32_t) rn << 16);
  }
  // основной переход
  for (uint8_t step_num = 0; step_num < total_steps; step_num++) {
    uint8_t rn = (r2 * step_num + r1 * (total_steps - 1 - step_num)) / (total_steps - 1);
    uint8_t gn = (g2 * step_num + g1 * (total_steps - 1 - step_num)) / (total_steps - 1);
    uint8_t bn = (b2 * step_num + b1 * (total_steps - 1 - step_num)) / (total_steps - 1);
    os_buffer[step_num + oversample] = bn + ( (uint32_t) gn << 8) + ((uint32_t) rn << 16);
  }
  // затухание
  for (uint8_t step_num = 0; step_num < oversample; step_num++) {
    uint8_t rn = (r2 * (oversample - step_num - 1)) / oversample;
    uint8_t gn = (g2 * (oversample - step_num - 1)) / oversample;
    uint8_t bn = (b2 * (oversample - step_num - 1)) / oversample;
    os_buffer[step_num + total_steps + oversample] = bn + ( (uint32_t) gn << 8) + ((uint32_t) rn << 16);
  }
}

// копирует буфер one_ray в заданный луч
void buffer_to_ray(uint8_t ray_num, uint8_t inverse) {
  if (ray_num >= ray_number) return;

  for (uint8_t i = 0; i < ray_pixel; i++) {
    if (inverse) {
      strip.setPixelColor(i + ray_num * ray_pixel, one_ray[ray_pixel - 1 - i]);
    } else {
      strip.setPixelColor(i + ray_num * ray_pixel, one_ray[i]);
    }
  }
}

// очистка данных луча
void clear_one_ray(void) {
  for (uint8_t i = 0; i < ray_pixel; i++) one_ray[i] = 0;
}

// очистка заданного луча
void clear_ray(uint8_t ray_num) {
  uint8_t curr = ray_num * ray_pixel;
  for (uint8_t i = 0; i < ray_pixel; i++) {
    strip.setPixelColor(curr++, 0x00000000);
  }
}

// очистка всего
void clear_all(void) {
  for (uint8_t i = 0; i < ray_number; i++) {
    clear_ray(i);
  }
}

// задать яркость
// сюда можно запихнуть чтение значения яркости от потенциометра по любому аналоговому входу
void SetBrightness(void) {
  strip.setBrightness(cur_br);
}

