////
//// This program is designated for testing the parallell SRAM/EEPROM chips with Arduino Mega (2560),
//// using the serial console as a command line interface.
////

// Default chip capacity (words). Can be changed with 'set cap' command.
long cap = 524288;

// Address pins, starting from pin A0
const int addr[] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 23, 25, 27};  

// Data IO pins, starting from pin DQ0 or I/O0
const int io[] = {31, 33, 35, 37, 39, 41, 43, 45};

// Chip select pin
#define cs 49

// Write enable pin
#define we 51

// Output enable pin
#define oe 53

// Write pulse width (tWP) - delay during the write sequence in milliseconds
// -1 = use potentiometer input as a reference.
// This value can be changed with 'set twp' command.
int twp = 1;

// Potentiometer input pin for dynamic tWP change (optional). Use only analog input pins A0 - A15.
#define twp_pin A7

// Buzzer anode pin, for sound signalization (optional)
#define buzz 10


const long conv_arr[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288};
long errors[1000];
int error_count = 0;
long conv_sub;
long s_addr;
long res_addr;
char ql;
char dl;
int data_dec;
String data_bin;
String read_bin;
String addr_bin;
String rev;
String serial_in = "";

void setup() {
  for (int a = 0; a < sizeof(addr)/sizeof(int); a++) { pinMode(addr[a], OUTPUT); }
  pinMode(cs, OUTPUT);
  pinMode(we, OUTPUT);
  pinMode(oe, OUTPUT);
  pinMode(twp_pin, INPUT);
  pinMode(buzz, OUTPUT);
  Serial.begin(115200);
  Serial.println("Ready.");
  delay(500);
}

void sound_alarm() {
  digitalWrite(buzz, HIGH);
  delay(150);
  digitalWrite(buzz, LOW);
  delay(50);

  digitalWrite(buzz, HIGH);
  delay(150);
  digitalWrite(buzz, LOW);
  delay(50);

  digitalWrite(buzz, HIGH);
  delay(150);
  digitalWrite(buzz, LOW);
  delay(50);

  digitalWrite(buzz, HIGH);
  delay(600);
  digitalWrite(buzz, LOW);
}

String dec_to_bin(long dec, char q) {
  // Convert decimal to binary

  if (sizeof(conv_arr)/sizeof(long) < q) {
    Serial.println("\nError: unable to convert to binary - max bit depth reached.");
    delay(1000);
    exit(0);
  }

  String bin = "";
  conv_sub = dec;
  for (int i = (sizeof(conv_arr)/sizeof(long))-1; i >= 0; i--) {
    if (conv_arr[i] <= conv_sub) {
      conv_sub = conv_sub - conv_arr[i];
      bin = bin + "1";
    }
    else { bin = bin + "0"; }
  }
  return(bin.substring(bin.length()-q, bin.length()));
}

String str_rev(String s) { 
  // Reverse the string
  rev = "";
  for (int i = 0; i < s.length(); i++) { rev = rev + s.substring(s.length()-1-i, s.length()-i); }
  return(rev);
}

void dump_errors() {
  // Print errors
  Serial.println("\nFound errors: " + String(error_count) );
  for (int i = 0; i < error_count; i++) { Serial.println("Error Word: " + String(errors[i])); }
}

char test_word(long a, int data) {
  // Write-read a single word
  data_bin = write_word(a, data);
  read_bin = read_word(a);
  Serial.print(", read binary: " + read_bin);

  if (data_bin == read_bin) { return(1); }
  else { return(0); }
}

String write_word(long a, int data) {

  // Setting Address
  addr_bin = dec_to_bin(a, ql);
  rev = str_rev(addr_bin);

  Serial.print("Word: " + String(a) + ", Addr: " + addr_bin);
  for (int i = 0; i < ql; i++) {
    if (String(rev[i]) == "1") { digitalWrite(addr[i], HIGH); }
    else { digitalWrite(addr[i], LOW); }
  }

  // Setting Data
  data_bin = dec_to_bin(data, 8);
  rev = str_rev(data_bin);
  Serial.print(", Data_dec: " + String(data) + ", Data_bin: " + data_bin);
  for (int i = 0; i < dl; i++) {
    pinMode(io[i], OUTPUT);
    if (String(rev[i]) == "1") { digitalWrite(io[i], HIGH); }
    else { digitalWrite(io[i], LOW); }
  }

  // Writing
  digitalWrite(cs, LOW);
  digitalWrite(we, LOW);
  if (twp == -1) { delay(map(analogRead(twp_pin), 10, 1000, 1, 100)); }
  else { delay(twp); }
  digitalWrite(we, HIGH);
  digitalWrite(cs, HIGH);
  return(data_bin);
}

String read_word(long a) {

  // Setting Address
  addr_bin = dec_to_bin(a, ql);
  rev = str_rev(addr_bin);
  for (int i = 0; i < ql; i++) {
    if (String(rev[i]) == "1") { digitalWrite(addr[i], HIGH); }
    else { digitalWrite(addr[i], LOW); }
  }

  // Reading Data
  read_bin = "";
  digitalWrite(cs, LOW);
  digitalWrite(oe, LOW);
  delay(1);
  for (int i = 0; i < dl; i++) {
    digitalWrite(io[i], LOW);
    pinMode(io[i], INPUT);
    if (digitalRead(io[i]) == 1) { read_bin = "1" + read_bin; }
    else { read_bin = "0" + read_bin; }
  }
  digitalWrite(oe, HIGH);
  digitalWrite(cs, HIGH);
  return(read_bin);
}


void loop() {
  
  ql = sizeof(addr)/sizeof(int);
  dl = sizeof(io)/sizeof(int);
  digitalWrite(oe, HIGH);
  digitalWrite(we, HIGH);

  if (Serial.available()) {   
      
    serial_in = String(Serial.readString());
    serial_in.remove(serial_in.length()-1, 1);                // trim CR
    char charArray[20]; 
    serial_in.toCharArray(charArray, serial_in.length() + 1); // convert string to char array
    char *str;
    char *c = charArray;
    String stack[20];
    int i = 0;
    // parse the input string
    while (str = strtok(c, " ")) {
      stack[i] = String(str);
      i++;
      c = NULL;
    }

    if (stack[0] == "start" || stack[0] == "resume") {    // Start check from word, or continue from the previous word (after stopping)
      if (stack[0] == "resume") { s_addr = res_addr; }
      else { 
        s_addr = stack[1].toInt(); 
        error_count = 0;
      } 
      if ((s_addr > 0 && s_addr < cap) || stack[1] == "0" || stack[0] == "resume") {
        for (long a = s_addr; a < cap; a++) {
          Serial.print("Errors: " + String(error_count) + ", tWP: " + String(twp) + ", ");
          data_dec = random(0, 256);
          if (test_word(a, data_dec) == 1) { Serial.println(" - OK"); }
          else {
            Serial.println(" - ERROR");
            errors[error_count] = a;
            error_count++;
            if (error_count == sizeof(errors)/sizeof(long)) {
              Serial.println(" Too many errors :-( ");
              break;
            }
          }
          if (Serial.available()) {   
            // check for stop condition
            serial_in = String(Serial.readString());
            serial_in.remove(serial_in.length()-1, 1);                // trim CR
            if (serial_in == "stop") {
              Serial.println("Stop command received.");
              res_addr = a + 1;
              break; 
            }
          }
        }
        Serial.println("Done!");
        if (error_count > 0) { dump_errors(); }
        sound_alarm();
        delay(1000);
      }
      else {
        Serial.println("Wrong address " + stack[1]);
        return 1;                
      }
    }

    else if (stack[0] == "recheck") {   // re-check all error words, found during the previous test
      if (error_count > 0) {
        Serial.print("Rechecking errors.");
        for (long i = 0; i < error_count; i++) {
          data_dec = random(0, 256);
          if (test_word(errors[i], data_dec) == 1) { Serial.println(" - OK"); }
          else { Serial.println(" - ERROR"); }
        }
        Serial.println("Done!");
        delay(1000);
      }
      else {
        Serial.println("No known errors from the last run.");
        return 1;                
      }
    }

    else if (stack[0] == "test") {      // Write-read single word
      s_addr = stack[1].toInt();
      if ((s_addr > 0 && s_addr < cap) || stack[1] == "0") {
        data_dec = random(0, 256);
        if (test_word(s_addr, data_dec) == 1) { Serial.println(" - OK"); }
        else { Serial.println(" - ERROR:"); }
      }
      else {
        Serial.println("Wrong address: " + stack[1]);
        return 1;
      }
    }

    else if (stack[0] == "write") {     // Write single word
      s_addr = stack[1].toInt();
      if ((s_addr > 0 && s_addr < cap) || stack[1] == "0") {
        if (stack[2].length() > 0 && stack[2].toInt() <= 255) { data_dec = stack[2].toInt(); }
        else {
          Serial.println("Using random data.");
          data_dec = random(0, 256); 
        }
        write_word(s_addr, data_dec);
        Serial.println("");
      }
      else {
        Serial.println("Wrong address: " + stack[1]);
        return 1;
      }
    }

    else if (stack[0] == "read") {      // Read single word
      s_addr = stack[1].toInt();
      if ((s_addr > 0 && s_addr < cap) || stack[1] == "0") {
        Serial.println("Word: " + String(s_addr) + ", Addr: " + dec_to_bin(s_addr, ql) + ", read binary: " + read_word(s_addr));
      }
      else {
        Serial.println("Wrong address: " + stack[1]);
        return 1;
      }
    }

    else if (stack[0] == "errors") {    // Show errors
      if (error_count > 0) { dump_errors(); }
      else { Serial.println("No known errors from the last run."); }
      delay(1000);
    }

    else if (stack[0] == "set") {       // Parameter settings
      if (stack[1] == "twp") {          // set writing cycle delay
        if (stack[2].toInt() > 0 ) { 
          twp = stack[2].toInt();
          Serial.println("tWP was set to: " + String(twp));
        }
        else if (stack[2].toInt() == -1) {
          twp = -1;
          Serial.println("tWP was set to potentiometer input.");
        }
        else { Serial.println("set twp: invalid tWP value."); }
      }
      else if (stack[1] == "cap") {     // set chip capacity
        char arr[12];
        stack[2].toCharArray(arr, sizeof(arr));
        long r_cap = atol(arr);
        if (r_cap > 0 ) { 
          cap = r_cap;
          Serial.println("Capacity was set to: " + String(cap) + " words.");
        }
        else { Serial.println("set cap: invalid value."); }
      }
      else { Serial.println("Set: wrong parameter."); }
    }

    else if (stack[0] == "overlap") {   // Address space overlap test

      // writing 
      s_addr = 1;
      int i = 0;
      String test_data[sizeof(addr)/sizeof(int)];
      Serial.println("Writing...");
      while (s_addr < cap) {
        data_dec = random(0, 256);
        data_bin = write_word(s_addr, data_dec);
        Serial.println("");
        test_data[i] = String(data_bin);
        s_addr = s_addr * 2;
        i++;
      }

      // reading
      s_addr = 1;
      i = 0;
      String r_bin;
      Serial.println("\nReading...");
      while (s_addr < cap) {
        r_bin = read_word(s_addr);
        Serial.print("Word: " + String(s_addr) + ", Addr: " + dec_to_bin(s_addr, ql) + ", read binary: " + r_bin);
        if (r_bin.equals(test_data[i])) { Serial.println(" - Match"); }
        else { Serial.println(" - DOESN'T MATCH!"); }
        s_addr = s_addr * 2;
        i++;
      }
    }

    else {
      Serial.println(stack[0] + ": Unknown command.");
    }
  } 
}
