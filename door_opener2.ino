
//Declare Pins
int y_led = 2; //digital
int g1_led = 12; //digital
int g2_led = 10; //digital
int r_led = 13; //digital

int b1 = 6; //analog (can't get digital read to work)
int b2 = A4; //digital
int b3 = A2; //digital
int b4 = A0; //digital

int mic = 7; //analog

int relay = 11; //digital

//Declare variables
int last_b1 = 1000;
boolean last_b2 = 1;
boolean last_b3 = 1;
boolean last_b4 = 1;

int current_b1;
boolean current_b2;
boolean current_b3;
boolean current_b4;

boolean y_led_status = 0;
boolean g1_led_status = 0;
boolean g2_led_status = 0;
boolean r_led_status = 0;

//mic Variables
int past_values[8] = { 170, 170, 170, 170, 170, 170, 170, 170};
int mic_value;
int array_index = 0;
int total_deviation = 0;

//timing variables
unsigned long time;
boolean last_mic_function;
int num_switches;


//delay variables
unsigned long g1_time, g2_time;

void setup() {                
  pinMode(y_led, OUTPUT);     
  pinMode(g1_led, OUTPUT); 
  pinMode(g2_led, OUTPUT); 
  pinMode(r_led, OUTPUT); 
  
  pinMode(b1, INPUT); 
  pinMode(b2, INPUT); 
  pinMode(b3, INPUT); 
  pinMode(b4, INPUT); 
  
  pinMode(mic, INPUT);
  
  pinMode(relay, OUTPUT);
  
  digitalWrite(y_led, LOW);
  digitalWrite(g1_led, LOW);
  digitalWrite(g2_led, LOW);
  digitalWrite(r_led, LOW);
  
  digitalWrite(relay, LOW);
  
}

void loop() {
  delay(5);
  
  current_b1 = analogRead(b1);
  current_b2 = digitalRead(b2);
  current_b3 = digitalRead(b3);
  current_b4 = digitalRead(b4);
  
  //power button
  if (r_led_status)
  {
  if (current_b1 < 500)
  {
    if (last_b1 > 500)
    {
      if (g1_led_status)
      {
        for(unsigned long i = millis() - g1_time + 3600000; i > 3600000; i = i - 3600000)
        {
          delay(500);
          digitalWrite(y_led, HIGH);
          delay(100);
          digitalWrite(y_led, LOW);
        }
      }
      
      if (g2_led_status)
      {
        for(unsigned long i = millis() - g2_time + 25200000; i > 3600000; i = i - 3600000)
        {
          delay(500);
          digitalWrite(y_led, HIGH);
          delay(100);
          digitalWrite(y_led, LOW);
        }
      }
      //trigger code
      /*
      if (y_led_status)
      {
        digitalWrite(y_led, LOW);
        y_led_status = 0;
      }
      else
      {
        digitalWrite(y_led, HIGH);
        y_led_status = 1;
      }
      */
    }
  }
  if (!current_b2)
  {
    if (last_b2)
    {
      if (g1_led_status)
      {
        digitalWrite(g1_led, LOW);
        g1_led_status = 0;
      }
      else
      {
        digitalWrite(g1_led, HIGH);
        g1_led_status = 1;
        
        g1_time = millis();
      }
      
      //turn off other delay
      if (g2_led_status)
      {
        g2_led_status = 0;
        digitalWrite(g2_led, LOW);
      }
    }
    
  }
  if (!current_b3)
  {
    if (last_b3)
    {
      if (g2_led_status)
      {
        digitalWrite(g2_led, LOW);
        g2_led_status = 0;
      }
      else
      {
        digitalWrite(g2_led, HIGH);
        g2_led_status = 1;
        
        g2_time = millis();
      }
      
      //turn off other delay
      if (g1_led_status)
      {
        g1_led_status = 0;
        digitalWrite(g1_led, LOW);
      }
    }
  }
  }
  if (!current_b4)
  {
    if (last_b4)
    {
      if (r_led_status)
      {
        digitalWrite(r_led, LOW);
        r_led_status = 0;
        
        //turn off other lights
        g1_led_status = 0;
        g2_led_status = 0;
        digitalWrite(g1_led, LOW);
        digitalWrite(g2_led, LOW);
      }
      else
      {
        digitalWrite(r_led, HIGH);
        r_led_status = 1;
      }
    }
  }
  
  if (micArrayUpdate(analogRead(mic)) && r_led_status && !g1_led_status && !g2_led_status)
  {
    time = millis();
    last_mic_function = 1;
    num_switches = 0;
    
    while(abs(millis()-time) < 2000) //this has a possibility of failing 1 in every 50 days
    {
      delay(10);
      if (last_mic_function != micArrayUpdate(analogRead(mic)))
      {
        ++num_switches;
        
        last_mic_function = !last_mic_function;
        
      }
      
      if (num_switches >=5)
      {
       digitalWrite(relay, HIGH);
        delay(2000);
       digitalWrite(relay, LOW); 
       
       if (last_mic_function == 0)
        {
            total_deviation = 0;
            for (int i = 0; i < 8; ++i)
            {
            past_values[i] = 170;
            } 
        }
      }
    }
  }
  
  //check times
  if (g1_led_status)//8 hour delay
  {
    if (g1_time > millis())
    {
        if (g1_time + 7200000 < millis() + 4294967295)
        {
          g1_led_status = 0;
          digitalWrite(g1_led, LOW);
        }
    }
    else if (g1_time + 7200000 < millis())
    {
      g1_led_status = 0;
      digitalWrite(g1_led, LOW);
    }
    
  }
  if (g2_led_status)//2 hour delay
  {
    if (g2_time > millis())
    {
        if (g2_time + 28800000 < millis() + 4294967295)
        {
          g2_led_status = 0;
          digitalWrite(g2_led, LOW);
        }
    }
    else if (g2_time + 28800000 < millis())
    {
      g2_led_status = 0;
      digitalWrite(g2_led, LOW);
    }
  }
  
  
  
  last_b1 = current_b1;
  last_b2 = current_b2;
  last_b3 = current_b3;
  last_b4 = current_b4;
}

bool micArrayUpdate(int new_value)
{
 if (array_index < 7)
 {
   ++array_index;
 }
 else if (array_index >= 7)
 {
  array_index = 0;
 }

 if (past_values[array_index] > 190 || past_values[array_index] < 150)
 {
   total_deviation = total_deviation - abs(170 - past_values[array_index]);
 }
 if (new_value > 190 || new_value < 150)
 {
   total_deviation = total_deviation + abs(170 - new_value);
 }
  past_values[array_index] = new_value;
  
  if (total_deviation > 50)
  {
    return 1; 
  }
  else
  {
    return 0;
  }
}
