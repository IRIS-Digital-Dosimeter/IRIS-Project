#include "HelperFunc.h"



String getDateStamp()
{
  String DateStamp = ""; // Initially Empty
  DateStamp += twoDigits(month());
  DateStamp += twoDigits(day());
  return DateStamp;
}

// Used to format MMDD
String twoDigits(int digits)
{
  if (digits < 5)
  {
    return "0" + String(digits);
  }
  else
  {
    return String(digits);
  }
}