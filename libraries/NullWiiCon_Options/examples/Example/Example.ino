#include <Arduino.h>
#include <NintendoExtensionCtrl.h>
#include <NullWiiCon_Options.h>

ClassicController classic;
NullWiiCon_Options options(6, 0, 100, 1000);

void setup() {
    // put your setup code here, to run once:
    classic.begin();
    options.begin();
}

void loop() {
    // Serial.println(preferences.peek_option);
    // put your main code here, to run repeatedly:
    if (classic.update()) { // Get new data!
        classic.fixNESThirdPartyData();
        options.menu_check(classic);
        if(options.in_menu){
            options.menu_work(classic);
        } else {
            if(options.lr_enable && classic.buttonA()){
                digitalWrite(6, HIGH);
            } else {
                digitalWrite(6, LOW);
            }
        }
    } else {
        classic.reconnect();
    }
}
