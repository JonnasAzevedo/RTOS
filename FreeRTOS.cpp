#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#define BATTERY_LEVEL_IN 35 // ESP32 analogic pin
#define ANALOGIC_RESOLUTION 4095 // The analogic board resolution, for example, ESP32 is 12 bit (from 0 to 4095)
#define REFERENCE_VOLTAGE 3.3 // The reference voltage, for example, ESP32 works in 3.3 V reference voltage by default
#define R1 3800.0 // resistance of R1 (3.8 KR)
#define R2 1000.0 // resistance of R2 (1 KR)
#define MAX_VOLTAGE 13.0 // The intented voltage, 13 V
#define AVAREGE_VOLTAGE 12.3 // The intented voltage, 12.3 V
#define MIM_VOLTAGE 11.9 // The intented voltage, 11.9 V

LiquidCrystal_I2C lcd(0x3F,16,2);
QueueHandle_t Queue_Voltage;

void xTask_Battery_Sensor(void *pvParameters){

  float Value;
  float Vout;
  float Vin;

  while (true)
  {
    
        Value = analogRead(BATTERY_LEVEL_IN); // Read value at analog input
        Vout = Value * (REFERENCE_VOLTAGE / ANALOGIC_RESOLUTION); // Calculate output voltage with rule of three formula
        Vin = Vout / (R2 / (R1 + R2)); // Calculate input voltage with voltage divider formula
        
        xQueueSend(Queue_Voltage, &Vin, 1000/portTICK_PERIOD_MS);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        
  }
                   
}

void xTask_LCD(void *pvParameters){
  
  float Vin = 0.0;
    
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Voltage:");

  lcd.setCursor(0, 1);
  lcd.print("Percent:");

  while(true)
  {
    
    xQueueReceive(Queue_Voltage, &Vin,  portMAX_DELAY);
 
    lcd.setCursor(8, 0);
    lcd.print(Vin, 1);
    
    if( Vin >= MAX_VOLTAGE )
       {
        
        lcd.setCursor(9, 1);
        lcd.print("100%  ");
        
       }
       
       else if( Vin < MAX_VOLTAGE && Vin > AVAREGE_VOLTAGE )
       {
        
        lcd.setCursor(9, 1);
        lcd.print("80%  ");
         
       }
        
       else if ( Vin > MIM_VOLTAGE && Vin < AVAREGE_VOLTAGE )
       {
          
        lcd.setCursor(9 ,1);
        lcd.print("50%  ");
         
        }
        
        else if ( Vin <= MIM_VOLTAGE){
          
        lcd.setCursor(9 ,1);
        lcd.print("0%  ");
          
          }
     
     }
      
}
  
  
    


void setup() {
  
 Serial.begin(115200);
 pinMode(BATTERY_LEVEL_IN, INPUT);
 
 Queue_Voltage = xQueueCreate(1, sizeof( float ));
 
 xTaskCreate(xTask_Battery_Sensor, "Battery Sensor", 4094, NULL, 1, NULL);
 xTaskCreate(xTask_LCD, "LCD", 4094, NULL, 2, NULL);

}

 




 
















void loop() {
  // There's nothing to be done here.
}
