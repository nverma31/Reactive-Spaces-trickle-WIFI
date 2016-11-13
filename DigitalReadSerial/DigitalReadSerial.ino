/*
   An Arduino controller for LED strings based on the SM16716 controller.

   By Mike Tsao (github.com/sowbug). Copyright 2012, all rights reserved.

   You might see these being sold as WS2801 strands, and you're wondering why
   your off-the-shelf WS2801 code isn't working. This sketch will restore
   your confidence that your lights aren't broken.

   This code isn't the fastest, but it is just about the simplest you'll
   find. For performance, investigate code that uses hardware SPI.

   Beware that everyone seems to use different wiring color schemes. I
   found the order of the wires is more reliable. Looking at the IC side
   of the board, you'll find from left to right: V+, Data, Clock, Ground.
*/


#define DATA_PIN (11)
#define CLOCK_PIN (12)

#define LIGHT_COUNT (100)

uint32_t pixels[LIGHT_COUNT];
int Matrix[10][10];

int loopcount = 0;
int  cont = 0;
int contblue = 0;
#define pinModeFast(x, y) pinMode(x, y)
#define digitalWriteFast(x, y) digitalWrite(x, y)
void setup() {

  int k = 0;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      if (i % 2 != 0) {
        Matrix[i][j] = 10 * i + j;
      } else {
        Matrix[i][j] = 10 * i + (10 - j - 1);
      }
    }
  }
  Serial.begin(9600);
  if (loopcount == 0) {
    for (int i = 0; i < 10; i++)
      for (int j = 0; j < 10; j++) {
        Serial.println(Matrix[i][j]);
      }
  }
  pinModeFast(DATA_PIN, OUTPUT);
  pinModeFast(CLOCK_PIN, OUTPUT);
  digitalWriteFast(DATA_PIN, LOW);
  digitalWriteFast(CLOCK_PIN, LOW);
}

static void set_pixel(uint8_t index, uint32_t color) {
  pixels[index] = color & 0x00FFFFFF;
}

static void set_pixel_rgb(uint8_t index, uint32_t r, uint32_t g, uint32_t b) {
  set_pixel(index, r | (g << 8) | (b << 16));
}

static void toggle_clock() {
  digitalWriteFast(CLOCK_PIN, HIGH);
  digitalWriteFast(CLOCK_PIN, LOW);
}

static void write_pixel(uint8_t i) {
  const uint32_t MASK = ((uint32_t)(1) << 24);
  uint32_t p = pixels[i] | MASK;
  int j = 25;
  while (j--) {
    digitalWriteFast(DATA_PIN, (p & MASK) ? HIGH : LOW);
    toggle_clock();
    p <<= 1;
  }
}

static void write_blank_pixel() {
  int j = 25;
  while (j--) {
    digitalWriteFast(DATA_PIN, 0);
    toggle_clock();
  }
}

static void show() {
  digitalWriteFast(DATA_PIN, LOW);
  for (int i = 0; i < 50; i++) {
    toggle_clock();
  }
  for (int i = 0; i < LIGHT_COUNT; ++i) {
    write_pixel(i);
  }
  write_blank_pixel();
  delay(1);
}

static void solid_color(uint32_t color) {
  for (int i = 0; i < LIGHT_COUNT; ++i) {
    set_pixel(i, color);
  }
}

static uint32_t simple_color(uint8_t color) {
  switch (color) {
    case 0:
      return 0x00ff0000;
    case 1:
      return 0x0000ff00;
    case 2:
      return 0x000000ff;
    case 3:
      return 0x00ffff00;
    case 4:
      return 0x00ff00ff;
    case 5:
      return 0x0000ffff;
    case 6:
      return 0x00ffffff;
    default:
      return 0x00330000;
  }
}

static uint32_t rand_color() {
  return simple_color(rand() % 7);
}

static uint32_t rand_color_except(uint32_t color) {
  uint32_t result_color;
  do {
    result_color = simple_color(rand() % 7);
  } while (result_color == color);
  return result_color;
}

static uint32_t get_dimmer_mask(uint32_t value) {
  return value | (value << 8) | (value << 16);
}

static void scroll(int8_t delta) {
  size_t bytes_to_move = (LIGHT_COUNT - delta) * sizeof(uint32_t);
  if (delta > 0) {
    memmove( & pixels[delta], & pixels[0], bytes_to_move);
  } else {
    memmove( & pixels[0], & pixels[-delta], bytes_to_move);
  }
}

static void shutdown(int col) {
  for (int i = 0; i < 10; i++) {
    set_pixel_rgb(Matrix[col][i], 0, 0, 0);
    show();
    delay(5);
  }
}

static void shutdown_final(int col) {
  for (int i = 9; i >= 0 ; i--) {
    set_pixel_rgb(Matrix[col][i], 0, 0, 0);
    show();
    delay(5);
  }
}

static void shutdown_reverse(int col) {
  for (int i = 9; i >= 0; i++) {
    set_pixel_rgb(Matrix[col][i], 0, 0, 0);
    show();
    delay(5);
  }
}

static void continuos_trickle(int col) {

  cont++;



  for (int j = 0; j <= 9; j++) {
    set_pixel_rgb(Matrix[3][j], 0, 0, 0);
    set_pixel_rgb(Matrix[4][j], 0, 0, 0);
    set_pixel_rgb(Matrix[5][j], 0, 0, 0);
    //                 set_pixel_rgb(Matrix[3][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[4][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[5][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[3][i+2], 0, 0, 0);
    //                set_pixel_rgb(Matrix[4][i+2], 0, 0, 0);
    //                set_pixel_rgb(Matrix[5][i+2], 0, 0, 0);
    show();

    set_pixel_rgb(Matrix[3][j], 255, 255, 255);
    set_pixel_rgb(Matrix[4][j], 255, 255, 255);
    set_pixel_rgb(Matrix[5][j], 255, 255, 255);


    //                set_pixel_rgb(Matrix[3][i+1], 255, 255, 255);
    //                 set_pixel_rgb(Matrix[4][i+1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[5][i+1], 255, 255, 255);
    //
    //                set_pixel_rgb(Matrix[3][i+2], 255, 255, 255);
    //                 set_pixel_rgb(Matrix[4][i+2], 255, 255, 255);
    //                set_pixel_rgb(Matrix[5][i+2], 255, 255, 255);
    show();




    //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);

    //                set_pixel_rgb(Matrix[1 + col][i], 255, 255, 255);
    //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
  }

}


static void continuos_trickle_blue(int col) {





  for (int j = 0; j <= 9; j++) {
    set_pixel_rgb(Matrix[col][j], 0, 0, 0);
    set_pixel_rgb(Matrix[col + 1][j], 0, 0, 0);
    set_pixel_rgb(Matrix[col + 2][j], 0, 0, 0);
    //                 set_pixel_rgb(Matrix[3][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[4][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[5][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[3][i+2], 0, 0, 0);
    //                set_pixel_rgb(Matrix[4][i+2], 0, 0, 0);
    //                set_pixel_rgb(Matrix[5][i+2], 0, 0, 0);
    show();

    set_pixel_rgb(Matrix[col][j], 255, 0, 0);
    set_pixel_rgb(Matrix[col + 1][j], 255, 0, 0);
    set_pixel_rgb(Matrix[col + 2][j], 255, 0, 0);
    cont++;

    //                set_pixel_rgb(Matrix[3][i+1], 255, 255, 255);
    //                 set_pixel_rgb(Matrix[4][i+1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[5][i+1], 255, 255, 255);
    //
    //                set_pixel_rgb(Matrix[3][i+2], 255, 255, 255);
    //                 set_pixel_rgb(Matrix[4][i+2], 255, 255, 255);
    //                set_pixel_rgb(Matrix[5][i+2], 255, 255, 255);
    show();




    //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);

    //                set_pixel_rgb(Matrix[1 + col][i], 255, 255, 255);
    //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
  }

}

static void continuos_trickle_red(int col) {





  for (int j = 0; j <= 9; j++) {
    set_pixel_rgb(Matrix[col][j], 0, 0, 0);
    set_pixel_rgb(Matrix[col + 1][j], 0, 0, 0);
    set_pixel_rgb(Matrix[col + 2][j], 0, 0, 0);
    //                 set_pixel_rgb(Matrix[3][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[4][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[5][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[3][i+2], 0, 0, 0);
    //                set_pixel_rgb(Matrix[4][i+2], 0, 0, 0);
    //                set_pixel_rgb(Matrix[5][i+2], 0, 0, 0);
    show();

    set_pixel_rgb(Matrix[col][j], 0, 0, 255);
    set_pixel_rgb(Matrix[col + 1][j], 0, 0, 255);
    set_pixel_rgb(Matrix[col + 2][j], 0, 0, 255);
    cont++;

    //                set_pixel_rgb(Matrix[3][i+1], 255, 255, 255);
    //                 set_pixel_rgb(Matrix[4][i+1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[5][i+1], 255, 255, 255);
    //
    //                set_pixel_rgb(Matrix[3][i+2], 255, 255, 255);
    //                 set_pixel_rgb(Matrix[4][i+2], 255, 255, 255);
    //                set_pixel_rgb(Matrix[5][i+2], 255, 255, 255);
    show();




    //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);

    //                set_pixel_rgb(Matrix[1 + col][i], 255, 255, 255);
    //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
  }

}

static void continuos_trickle_green(int col) {





  for (int j = 0; j <= 9; j++) {
    set_pixel_rgb(Matrix[col][j], 0, 0, 0);
    set_pixel_rgb(Matrix[col + 1][j], 0, 0, 0);
    set_pixel_rgb(Matrix[col + 2][j], 0, 0, 0);
    //                 set_pixel_rgb(Matrix[3][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[4][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[5][i+1], 0, 0, 0);
    //                set_pixel_rgb(Matrix[3][i+2], 0, 0, 0);
    //                set_pixel_rgb(Matrix[4][i+2], 0, 0, 0);
    //                set_pixel_rgb(Matrix[5][i+2], 0, 0, 0);
    show();

    set_pixel_rgb(Matrix[col][j], 0, 255, 0);
    set_pixel_rgb(Matrix[col + 1][j], 0, 255, 0);
    set_pixel_rgb(Matrix[col + 2][j], 0, 255, 0);
    cont++;

    //                set_pixel_rgb(Matrix[3][i+1], 255, 255, 255);
    //                 set_pixel_rgb(Matrix[4][i+1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[5][i+1], 255, 255, 255);
    //
    //                set_pixel_rgb(Matrix[3][i+2], 255, 255, 255);
    //                 set_pixel_rgb(Matrix[4][i+2], 255, 255, 255);
    //                set_pixel_rgb(Matrix[5][i+2], 255, 255, 255);
    show();




    //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);

    //                set_pixel_rgb(Matrix[1 + col][i], 255, 255, 255);
    //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
    //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
  }

}



static void trickle(int col) {
  if (col < 10) {
    for (int i = 0; i < 10; i++) {
      if (col == 0) {
        set_pixel_rgb(Matrix[col][i], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);

        //                set_pixel_rgb(Matrix[1 + col][i], 255, 255, 255);
        //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
        //                set_pixel_rgb(Matrix[1 + col][i + 1], 255, 255, 255);
      } else {
        set_pixel_rgb(Matrix[col][i], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col][i + 1], 255, 255, 255);

      }
      if (col > 2) {
        set_pixel_rgb(Matrix[col - 3][i], 0, 0, 0);
      }

      show();
      delay(5);
    }
  }


}

static void reverse_trickle(int col) {
  if (col < 10) {
    for (int i = 9; i >= 0; i--) {
      if (col == 9) {
        set_pixel_rgb(Matrix[col][i], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col][i - 1], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col][i - 1], 255, 255, 255);

        //                set_pixel_rgb(Matrix[col -1][i], 255, 255, 255);
        //                set_pixel_rgb(Matrix[ col -1][i - 1], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col-1][i - 1], 255, 255, 255);
      } else {
        set_pixel_rgb(Matrix[col][i], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col][i -1 ], 255, 255, 255);
        //                set_pixel_rgb(Matrix[col][i - 1], 255, 255, 255);

      }
      if (col < 7) {
        set_pixel_rgb(Matrix[col + 3][i], 0, 0, 0);
      }

      show();
      delay(5);
    }
  }


}

static void wipe(uint8_t delay_msec) {
  for (int i = 0; i < LIGHT_COUNT; ++i) {
    scroll(1);
    set_pixel(0, 0);
    show();
    delay(delay_msec);
  }
}
static void facebook() {
   uint32_t color = 0;
  int i = 0;
  if (loopcount == 0) {
    trickle(0);
    loopcount = 1;
  }
  if (loopcount == 1) {
    trickle(1);
    loopcount = 2;
  }
  if (loopcount == 2) {
    trickle(2);
    loopcount = 3;
  }
  if (loopcount == 3) {
    trickle(3);
    loopcount = 4;
  }
  if (loopcount == 4) {
    trickle(4);

    loopcount = 5;
  }
  if (loopcount == 5) {
    trickle(5);

    loopcount = 6;
  }
  if (loopcount == 6) {
    trickle(6);

    loopcount = 7;
  }
  if (loopcount == 7) {
    trickle(7);

    loopcount = 8;
  }
  if (loopcount == 8) {
    trickle(8);
    loopcount = 9;
  }
  if (loopcount == 9) {

    trickle(9);
    shutdown(7);
    shutdown(8);

    loopcount = 10;
    set_pixel_rgb(Matrix[0][0], 0, 0, 0);
    set_pixel_rgb(Matrix[9][9], 0, 0, 0);
    show();


  }
  //reverse
  if (loopcount == 10) {
    reverse_trickle(9);
    loopcount = 12;
  }
  if (loopcount == 12) {
    reverse_trickle(8);
    loopcount = 13;
  }
  if (loopcount == 13) {
    reverse_trickle(7);
    loopcount = 14;
  }
  if (loopcount == 14) {
    reverse_trickle(6);
    loopcount = 15;
  }
  if (loopcount == 15) {
    reverse_trickle(5);

    loopcount = 16;
  }
  if (loopcount == 16) {
    reverse_trickle(4);

    loopcount = 23;
  }
  if (loopcount == 17) {
    reverse_trickle(3);
    loopcount == 23;
  }

  //
  if (loopcount == 23 && cont < 5) {
    reverse_trickle(3);

    for (int i = 0; i < 10; i++) {
      set_pixel_rgb(Matrix[3][i], 255, 255, 255);
      set_pixel_rgb(Matrix[4][i], 255, 255, 255);
      set_pixel_rgb(Matrix[5][i], 255, 255, 255);
      show();
    }

    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    loopcount = 24;
  }

  if (loopcount == 24 && contblue < 10) {
    for (int i = 0; i < 10; i++) {
      set_pixel_rgb(Matrix[3][i], 255, 0, 0);
      set_pixel_rgb(Matrix[4][i], 255, 0, 0);
      set_pixel_rgb(Matrix[5][i], 255, 0, 0);
      show();
    }
    for (int k = 0 ; k < 5; k++) {
      continuos_trickle_blue(3);
      continuos_trickle_blue(3);
      continuos_trickle_blue(3);

    }
    shutdown(3);

    continuos_trickle_blue(4);
    shutdown(4);

    continuos_trickle_blue(5);
    shutdown(5);


    continuos_trickle_blue(6);
    shutdown(6);

    for (int k = 0; k < 10; k++) {
      continuos_trickle_blue(7);
    }
    shutdown_final(9);
    shutdown_final(8);
    shutdown_final(7);



    loopcount = 28;

  }
  if (loopcount == 28) {

  }

  if (loopcount == 18) {
    reverse_trickle(2);

    loopcount = 19 ;
  }
  if (loopcount == 19) {
    reverse_trickle(1);
    loopcount = 20;
  }
  if (loopcount == 20) {
    reverse_trickle(0);
    shutdown_reverse(2);
    shutdown_reverse(1);
    shutdown_reverse(0);
    loopcount = 28;

  }
}

static void amazon() {
   uint32_t color = 0;
  int i = 0;
  if (loopcount == 0) {
    trickle(0);
    loopcount = 1;
  }
  if (loopcount == 1) {
    trickle(1);
    loopcount = 2;
  }
  if (loopcount == 2) {
    trickle(2);
    loopcount = 3;
  }
  if (loopcount == 3) {
    trickle(3);
    loopcount = 4;
  }
  if (loopcount == 4) {
    trickle(4);

    loopcount = 5;
  }
  if (loopcount == 5) {
    trickle(5);

    loopcount = 6;
  }
  if (loopcount == 6) {
    trickle(6);

    loopcount = 7;
  }
  if (loopcount == 7) {
    trickle(7);

    loopcount = 8;
  }
  if (loopcount == 8) {
    trickle(8);
    loopcount = 9;
  }
  if (loopcount == 9) {

    trickle(9);
    shutdown(7);
    shutdown(8);

    loopcount = 10;
    set_pixel_rgb(Matrix[0][0], 0, 0, 0);
    set_pixel_rgb(Matrix[9][9], 0, 0, 0);
    show();


  }
  //reverse
  if (loopcount == 10) {
    reverse_trickle(9);
    loopcount = 12;
  }
  if (loopcount == 12) {
    reverse_trickle(8);
    loopcount = 13;
  }
  if (loopcount == 13) {
    reverse_trickle(7);
    loopcount = 14;
  }
  if (loopcount == 14) {
    reverse_trickle(6);
    loopcount = 15;
  }
  if (loopcount == 15) {
    reverse_trickle(5);

    loopcount = 16;
  }
  if (loopcount == 16) {
    reverse_trickle(4);

    loopcount = 23;
  }
  if (loopcount == 17) {
    reverse_trickle(3);
    loopcount == 23;
  }

  //
  if (loopcount == 23 && cont < 5) {
    reverse_trickle(3);

    for (int i = 0; i < 10; i++) {
      set_pixel_rgb(Matrix[3][i], 255, 255, 255);
      set_pixel_rgb(Matrix[4][i], 255, 255, 255);
      set_pixel_rgb(Matrix[5][i], 255, 255, 255);
      show();
    }

    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    loopcount = 24;
  }

  if (loopcount == 24 && contblue < 10) {
    for (int i = 0; i < 10; i++) {
      set_pixel_rgb(Matrix[3][i], 255, 0, 0);
      set_pixel_rgb(Matrix[4][i], 255, 0, 0);
      set_pixel_rgb(Matrix[5][i], 255, 0, 0);
      show();
    }
    for (int k = 0 ; k < 5; k++) {
      continuos_trickle_red(3);
      continuos_trickle_red(3);
      continuos_trickle_red(3);

    }
    shutdown(3);

    continuos_trickle_red(4);
    shutdown(4);

    continuos_trickle_red(5);
    shutdown(5);


    continuos_trickle_red(6);
    shutdown(6);

    for (int k = 0; k < 10; k++) {
      continuos_trickle_red(7);
    }
    shutdown_final(9);
    shutdown_final(8);
    shutdown_final(7);



    loopcount = 28;

  }
  if (loopcount == 28) {

  }

  if (loopcount == 18) {
    reverse_trickle(2);

    loopcount = 19 ;
  }
  if (loopcount == 19) {
    reverse_trickle(1);
    loopcount = 20;
  }
  if (loopcount == 20) {
    reverse_trickle(0);
    shutdown_reverse(2);
    shutdown_reverse(1);
    shutdown_reverse(0);
    loopcount = 28;

  }
}



static void google() {
   uint32_t color = 0;
  int i = 0;
  if (loopcount == 0) {
    trickle(0);
    loopcount = 1;
  }
  if (loopcount == 1) {
    trickle(1);
    loopcount = 2;
  }
  if (loopcount == 2) {
    trickle(2);
    loopcount = 3;
  }
  if (loopcount == 3) {
    trickle(3);
    loopcount = 4;
  }
  if (loopcount == 4) {
    trickle(4);

    loopcount = 5;
  }
  if (loopcount == 5) {
    trickle(5);

    loopcount = 6;
  }
  if (loopcount == 6) {
    trickle(6);

    loopcount = 7;
  }
  if (loopcount == 7) {
    trickle(7);

    loopcount = 8;
  }
  if (loopcount == 8) {
    trickle(8);
    loopcount = 9;
  }
  if (loopcount == 9) {

    trickle(9);
    shutdown(7);
    shutdown(8);

    loopcount = 10;
    set_pixel_rgb(Matrix[0][0], 0, 0, 0);
    set_pixel_rgb(Matrix[9][9], 0, 0, 0);
    show();


  }
  //reverse
  if (loopcount == 10) {
    reverse_trickle(9);
    loopcount = 12;
  }
  if (loopcount == 12) {
    reverse_trickle(8);
    loopcount = 13;
  }
  if (loopcount == 13) {
    reverse_trickle(7);
    loopcount = 14;
  }
  if (loopcount == 14) {
    reverse_trickle(6);
    loopcount = 15;
  }
  if (loopcount == 15) {
    reverse_trickle(5);

    loopcount = 16;
  }
  if (loopcount == 16) {
    reverse_trickle(4);

    loopcount = 23;
  }
  if (loopcount == 17) {
    reverse_trickle(3);
    loopcount == 23;
  }

  //
  if (loopcount == 23 && cont < 5) {
    reverse_trickle(3);

    for (int i = 0; i < 10; i++) {
      set_pixel_rgb(Matrix[3][i], 255, 255, 255);
      set_pixel_rgb(Matrix[4][i], 255, 255, 255);
      set_pixel_rgb(Matrix[5][i], 255, 255, 255);
      show();
    }

    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    continuos_trickle(0);
    loopcount = 24;
  }

  if (loopcount == 24 && contblue < 10) {
    for (int i = 0; i < 10; i++) {
      set_pixel_rgb(Matrix[3][i], 255, 0, 0);
      set_pixel_rgb(Matrix[4][i], 255, 0, 0);
      set_pixel_rgb(Matrix[5][i], 255, 0, 0);
      show();
    }
    for (int k = 0 ; k < 5; k++) {
      continuos_trickle_green(3);
      continuos_trickle_green(3);
      continuos_trickle_green(3);

    }
    shutdown(3);

    continuos_trickle_green(4);
    shutdown(4);

    continuos_trickle_green(5);
    shutdown(5);


    continuos_trickle_green(6);
    shutdown(6);

    for (int k = 0; k < 10; k++) {
      continuos_trickle_green(7);
    }
    shutdown_final(9);
    shutdown_final(8);
    shutdown_final(7);



    loopcount = 28;

  }
  if (loopcount == 28) {

  }

  if (loopcount == 18) {
    reverse_trickle(2);

    loopcount = 19 ;
  }
  if (loopcount == 19) {
    reverse_trickle(1);
    loopcount = 20;
  }
  if (loopcount == 20) {
    reverse_trickle(0);
    shutdown_reverse(2);
    shutdown_reverse(1);
    shutdown_reverse(0);
    loopcount = 28;

  }
}



void loop() {

   char inByte = 0;
  
  if(Serial.available()){ // only send data back if data has been sent
    inByte = Serial.read(); // read the incoming data
    
    
    if (inByte == '5') {
google();
    }
     if (inByte == '9') {
amazon();
    }

     if (inByte == '7') {
facebook();
    }
  }
}
