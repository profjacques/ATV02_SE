if(digitalRead(buttonPin) == LOW) {
  mode = (mode + 1) % 2;
  lcd.clear();
}
