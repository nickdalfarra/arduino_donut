const int potPin1 = 0; const int potPin2 = 1;   // input pins from the potentiometer
float alpha = 0; float beta = 0;  // X and Z axis rotaion angles

const int screen_width = 25; const int screen_height = 25;
const float theta_spacing = 0.07; // 0.07;
const float phi_spacing   = 0.02; // 0.02;

const byte R1 = 1; const byte R2 = 2; const byte K2 = 5;
//const float K1 = screen_width * K2 * 3 / (8 * (R1 + R2));
const float K1 = screen_width * K2 / (4 * (R1 + R2));

const float zmin = K2 - R1 - R2;
const float zmax = K2 + R1 + R2;
const float factor = 254 * zmin * zmax / (2 * (R1 + R2)); // for scaling ooz


// declare arrays
char lum_map[12] = {'.', ',', '-', '~', ':', ';', '=', '!', '*', '#', '$', '@'};
byte zbuffer[screen_width][screen_height];
char output[screen_width][screen_height];

void setup() {
  Serial.begin(9600);
  delay(500);
}

void loop() {
  alpha = analogRead(potPin1) * PI / 1024; // map potentiometer from 0 to 2*pi
  beta = analogRead(potPin2) * PI / 1024;
  render_frame(alpha, beta);
  //delay(750);
}

void render_frame(float A, float B) {
  // Given X and Z axis rotations, plot the ASCII torus.
  float cosA = cos(A); float sinA = sin(A); // precompute sin and cosine
  float cosB = cos(B); float sinB = sin(A); //sqrt(1 - pow(cosB,2));

  // reset arrays
  for (int i = 0; i < screen_width; i++) {
    for (int j = 0; j < screen_height; j++) {
      output[i][j] = ' ';
    }
  }
  memset(zbuffer, 0, sizeof(zbuffer));

  for (float theta = 0; theta < TWO_PI; theta += theta_spacing) {
    float costheta = cos(theta), sintheta = sin(theta); // precompute

    for (float phi = 0; phi < TWO_PI; phi += phi_spacing) { // inner loop
      float cosphi = cos(phi); float sinphi = sin(phi);

      float circlex = R2 + R1 * costheta;
      float circley = R1 * sintheta;

      float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
      float y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
      float z = K2 + cosA * circlex * sinphi + circley * sinA;
      float ooz = 1 / z; // "one over z"

      byte ooz_byte = (ooz - 1 / zmax ) * factor + 1;

      int xp = (int) (screen_width / 2 + K1 * ooz * x);
      int yp = (int) (screen_height / 2 - K1 * ooz * y);

      float L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);

      if (L > 0) {
        if (ooz_byte > zbuffer[xp][yp]) {
          zbuffer[xp][yp] = ooz_byte;
          //Serial.println(ooz_byte);
          int luminance_index = L * 8;
          output[xp][yp] = lum_map[luminance_index];
          //Serial.println(output[xp][yp]);
        }
      }
    }
  }

  for (int n = 0; n < 10; n++) {
    Serial.print('\n'); // clear the serial
  }

  for (int i = 0; i < screen_height; i++) {
    for (int j = 0; j < screen_width; j++) {
      Serial.print(output[i][j]);
    }
    Serial.println(' ');
  }
}

/*
      for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
          Serial.print(output[i][j]);
        }
        Serial.println(' ');
      }

*/
