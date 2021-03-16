#include <Tone32.h>

#ifndef MELODY
#define MELODY

// https://github.com/ShivamJoker/twinkle-twinkle-melody-esp32/blob/master/twinkle-twinkle.ino

#define BUZZER_PIN PIN_AC_TONE_MELODY
#define BUZZER_CHANNEL 0

void playMelody()
{
    // twinkle twinkle little star

    yield();
    tone(BUZZER_PIN, NOTE_C4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_C4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_A4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_A4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(650);

    //   How i wonder what you are

    yield();
    tone(BUZZER_PIN, NOTE_F4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_F4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_E4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_E4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_D4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_D4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_C4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(650);

    // up above the world so high

    yield();
    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_F4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_F4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_E4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_E4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_D4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(650);

    // like a diamond in the sky

    yield();
    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_F4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_F4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_E4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_E4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_D4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(650);

    // twinkle twinkle little star

    yield();
    tone(BUZZER_PIN, NOTE_C4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_C4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_A4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_A4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_G4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(650);

    //   How i wonder what you are

    yield();
    tone(BUZZER_PIN, NOTE_F4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_F4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_E4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_E4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_D4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_D4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(100);

    tone(BUZZER_PIN, NOTE_C4, 650, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(650);
    digitalWrite(BUZZER_PIN, LOW);
    // end of song
}

#endif
