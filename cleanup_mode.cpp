#include "app.h"
#include "freetronicsLCDShield.h"
#include "Stepper.h"
#include "mbed.h"


extern freetronicsLCDShield lcd;
extern Stepper StepMotor;

const int cfg_discharge_cycles = 10;
static int _current_discharge_cycle_count = 1;


TricklerState_t cleanup_mode_menu(void){
    lcd.cls();
    lcd.setCursorPosition(0, 0);
    lcd.printf("Close Hopper");

    lcd.setCursorPosition(1, 0);
    lcd.printf("> Press Select");

    return CLEANUP_MODE_WAIT_FOR_INPUT;
}

TricklerState_t cleanup_mode_wait_for_input(freetronicsLCDShield::ButtonType_t *button_press){
    // If select is pressed then go to cleanup charge/discharge mode
    if (*button_press == freetronicsLCDShield::BTN_SELECT){
        StepMotor.enableHold(true);
        _current_discharge_cycle_count = 1;
        return CLEANUP_MODE_WAIT_FOR_COMPLETE;
    }

    // Otherwise repeat the same state until the button is pressed
    return CLEANUP_MODE_WAIT_FOR_INPUT;
}


TricklerState_t cleanup_mode_wait_for_complete(void){
    // Run discharge cycle
    lcd.cls();
    lcd.setCursorPosition(0, 0);
    lcd.printf("Discharging");
    lcd.setCursorPosition(1, 0);
    lcd.printf("%d/%d cycles", _current_discharge_cycle_count, cfg_discharge_cycles);

    StepMotor.step(170);
    ThisThread::sleep_for(500ms);

    lcd.cls();
    lcd.setCursorPosition(0, 0);
    lcd.printf("Charging");

    lcd.setCursorPosition(1, 0);
    lcd.printf("%d/%d cycles", _current_discharge_cycle_count, cfg_discharge_cycles);

    StepMotor.step(-170);
    ThisThread::sleep_for(500ms);

    // Determine stop condition
    _current_discharge_cycle_count += 1;
    if (_current_discharge_cycle_count > cfg_discharge_cycles){
        _current_discharge_cycle_count = 1;

        StepMotor.enableHold(false);

        lcd.cls();
        lcd.setCursorPosition(0, 0);
        lcd.printf("Complete");
        ThisThread::sleep_for(1s);

        return MAIN_MENU;
    }

    return CLEANUP_MODE_WAIT_FOR_COMPLETE;
}