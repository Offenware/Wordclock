#include <Arduino.h>
#include "nederlands.h"
#include "wordclock.h"

//#define DEBUG

int indexje = 0;

/**
 * Loads a word code into the time matrix
 * @param  clockword code          a tupla defining the leds to be lit to form a word
 * @param  unsigned  int * matrix  LED matrix array representation
 */
void loadCode(clockword code, unsigned int * matrix) {

  // matrix = time_pattern
  matrix[code.row] = matrix[code.row] | code.positions;
   
}

void loadLanguageDutch(byte year, byte month, byte day, byte hour, byte minute, byte second, unsigned int * matrix, boolean secondenAan, String phaseName, uint8_t hetIs, uint8_t desOchtends, uint8_t maan) {

  if (hetIs) {
    loadCode((clockword) NL_HET, matrix);
    loadCode((clockword) NL_IS, matrix);
  }

  if (desOchtends) {
    loadCode((clockword) NL_DES, matrix);
  } else {  
    loadCode((clockword) NL_IN, matrix);
    loadCode((clockword) NL_DE, matrix);
  }

  int hour_corr = hour;
  int minute_corr = minute;
  int sec_corr = second;
  
  if (minute > 20) {
    hour_corr++;
  }

  if (hour_corr > 23) {
    hour_corr -= 24;
  }

  if (secondenAan and ((minute_corr > 39 and minute_corr <= 59) or (minute_corr > 20 and minute_corr < 30))) {
    sec_corr = 60 - sec_corr;
    if (sec_corr == 60) {
      sec_corr = 0;
    } 
    if (sec_corr != 0) {
      minute_corr++;
    } 
    if (minute_corr == 60) {
      minute_corr = 0;
    }
  }

  int sec_t = sec_corr / 10;
  int sec_d = sec_corr % 10;
  
  if (minute_corr != 15 and minute_corr != 45  and minute_corr != 30 and minute_corr != 0) {
    if (minute_corr == 1 or minute_corr == 31 or minute_corr == 29 or minute_corr == 59) {
        loadCode((clockword) NL_MINUUT, matrix);
    } else {
        loadCode((clockword) NL_MINUTEN, matrix);
    }
  }

  if (!secondenAan and minute_corr == 0) {
    loadCode((clockword) NL_UUR, matrix);
  }

  if ((minute_corr > 0 and minute_corr < 21) or (minute_corr > 30 and minute_corr < 40) or (secondenAan and ((minute_corr == 0 and minute == 0) or (minute_corr == 30 and minute == 30)))) {
    loadCode((clockword) NL_OVER, matrix);
  }

  if ((minute_corr > 39 and minute_corr <= 59) or (minute_corr > 20 and minute_corr < 30) or (secondenAan and ((minute_corr == 0 and minute == 59) or (minute_corr == 30 and minute == 29)))) {
    loadCode((clockword) NL_VOOR, matrix);
  }

  if (minute_corr > 20 and minute_corr < 40) {
    loadCode((clockword) NL_HALF, matrix);
  }

// Ongecorrigeerde uren gebruiken, anders zegt hij bij 5 voor half twaalf al dat het nacht of middag is, vanwege de twaalf
  if (hour >= 0 and hour < 6) {
    if (desOchtends) {
      loadCode((clockword) NL_NACHTS, matrix);
    } else {  
      loadCode((clockword) NL_NACHT, matrix);
    }
  }

  if (hour >= 6 and hour < 12) {
    if (desOchtends) {
      loadCode((clockword) NL_OCHTENDS, matrix);
    } else {  
      loadCode((clockword) NL_OCHTEND, matrix);
    }
  }

  if (hour >= 12 and hour < 18) {
    if (desOchtends) {
      loadCode((clockword) NL_MIDDAGS, matrix);
    } else {  
      loadCode((clockword) NL_MIDDAG, matrix);
    }
  }

  if (hour >= 18 and hour < 24) {
    if (desOchtends) {
      loadCode((clockword) NL_AVONDS, matrix);
    } else {  
      loadCode((clockword) NL_AVOND, matrix);
    }
  }

  boolean show_seconden = true;
  if (minute_corr == 15 or minute_corr == 45) {
    show_seconden = false;
  }

  if (show_seconden and secondenAan) {
    if (minute_corr != 0 and minute_corr != 30) {
      loadCode((clockword) NL_EN, matrix);
    }
    if (sec_corr == 1) {
        loadCode((clockword) NL_SECONDE, matrix);
    } else {
        loadCode((clockword) NL_SECONDEN, matrix);
    }
  }

  if (secondenAan) {
    switch (sec_t) {
      case 0:
        loadCode((clockword) NL_SEC_T_0, matrix);
        break;
        
      case 1:
        loadCode((clockword) NL_SEC_T_1, matrix);
        break;
        
      case 2:
        loadCode((clockword) NL_SEC_T_2, matrix);
        break;
        
      case 3:
        loadCode((clockword) NL_SEC_T_3, matrix);
        break;
        
      case 4:
        loadCode((clockword) NL_SEC_T_4, matrix);
        break;
        
      case 5:
        loadCode((clockword) NL_SEC_T_5, matrix);
        break;
    }
  
    switch (sec_d) {
      case 0:
        loadCode((clockword) NL_SEC_D_0, matrix);
        break;
        
      case 1:
        loadCode((clockword) NL_SEC_D_1, matrix);
        break;
        
      case 2:
        loadCode((clockword) NL_SEC_D_2, matrix);
        break;
        
      case 3:
        loadCode((clockword) NL_SEC_D_3, matrix);
        break;
        
      case 4:
        loadCode((clockword) NL_SEC_D_4, matrix);
        break;
        
      case 5:
        loadCode((clockword) NL_SEC_D_5, matrix);
        break;
        
      case 6:
        loadCode((clockword) NL_SEC_D_6, matrix);
        break;
        
      case 7:
        loadCode((clockword) NL_SEC_D_7, matrix);
        break;
        
      case 8:
        loadCode((clockword) NL_SEC_D_8, matrix);
        break;
        
      case 9:
        loadCode((clockword) NL_SEC_D_9, matrix);
        break;
    }
  }

  switch (minute_corr) {
    case 1:
    case 29:
    case 31:
    case 59:
      loadCode((clockword) NL_M_EEN, matrix);
      break;

    case 2:
    case 28:
    case 32:
    case 58:
      loadCode((clockword) NL_M_TWEE, matrix);
      break;

    case 3:
    case 27:
    case 33:
    case 57:
      loadCode((clockword) NL_M_DRIE, matrix);
      break;

    case 4:
    case 26:
    case 34:
    case 56:
      loadCode((clockword) NL_M_VIER, matrix);
      break;

    case 5:
    case 25:
    case 35:
    case 55:
      loadCode((clockword) NL_M_VIJF, matrix);
      break;

    case 6:
    case 24:
    case 36:
    case 54:
      loadCode((clockword) NL_M_ZES, matrix);
      break;

    case 7:
    case 23:
    case 37:
    case 53:
      loadCode((clockword) NL_M_ZEVEN, matrix);
      break;

    case 8:
    case 22:
    case 38:
    case 52:
      loadCode((clockword) NL_M_ACHT, matrix);
      break;

    case 9:
    case 21:
    case 39:
    case 51:
      loadCode((clockword) NL_M_NEGEN, matrix);
      break;

    case 10:
    case 50:
      loadCode((clockword) NL_M_TIEN, matrix);
      break;

    case 11:
    case 49:
      loadCode((clockword) NL_M_ELF, matrix);
      break;

    case 12:
    case 48:
      loadCode((clockword) NL_M_TWAALF, matrix);
      break;

    case 13:
    case 47:
      loadCode((clockword) NL_M_DERTIEN, matrix);
      break;

    case 14:
    case 46:
      loadCode((clockword) NL_M_VEERTIEN, matrix);
      break;

    case 15:
    case 45:
      loadCode((clockword) NL_M_KWART, matrix);
      break;

    case 16:
    case 44:
      loadCode((clockword) NL_M_ZESTIEN, matrix);
      break;

    case 17:
    case 43:
      loadCode((clockword) NL_M_ZEVENTIEN, matrix);
      break;

    case 18:
    case 42:
      loadCode((clockword) NL_M_ACHTTIEN, matrix);
      break;

    case 19:
    case 41:
      loadCode((clockword) NL_M_NEGENTIEN, matrix);
      break;

    case 20:
    case 40:
      loadCode((clockword) NL_M_TWINTIG, matrix);
      break;

  }

  switch (hour_corr) {
    case 0:
    case 12:
    case 24:
      loadCode((clockword) NL_H_TWAALF, matrix);
      break;

    case 1:
    case 13:
      loadCode((clockword) NL_H_EEN, matrix);
      break;

    case 2:
    case 14:
      loadCode((clockword) NL_H_TWEE, matrix);
      break;

    case 3:
    case 15:
      loadCode((clockword) NL_H_DRIE, matrix);
      break;

    case 4:
    case 16:
      loadCode((clockword) NL_H_VIER, matrix);
      break;

    case 5:
    case 17:
      loadCode((clockword) NL_H_VIJF, matrix);
      break;

    case 6:
    case 18:
      loadCode((clockword) NL_H_ZES, matrix);
      break;

    case 7:
    case 19:
      loadCode((clockword) NL_H_ZEVEN, matrix);
      break;

    case 8:
    case 20:
      loadCode((clockword) NL_H_ACHT, matrix);
      break;

    case 9:
    case 21:
      loadCode((clockword) NL_H_NEGEN, matrix);
      break;

    case 10:
    case 22:
      loadCode((clockword) NL_H_TIEN, matrix);
      break;

    case 11:
    case 23:
      loadCode((clockword) NL_H_ELF, matrix);
      break;
  }

  if (maan) {
    if (phaseName == "Nieuwe maan" ) {
      loadCode((clockword) NL_NM, matrix);
    }      
    if (phaseName == "Wassende sikkel" ) {
      loadCode((clockword) NL_NM_EK, matrix);
    }      
    if (phaseName == "Eerste kwartier" ) {
      loadCode((clockword) NL_EK, matrix);
    }      
    if (phaseName == "Wassende maan" ) {
      loadCode((clockword) NL_EK_VM, matrix);
    }      
    if (phaseName == "Volle maan" ) {
      loadCode((clockword) NL_VM, matrix);
    }      
    if (phaseName == "Afnemende maan" ) {
      loadCode((clockword) NL_VM_LK, matrix);
    }      
    if (phaseName == "Laatste kwartier" ) {
      loadCode((clockword) NL_LK, matrix);
    }      
    if (phaseName == "Afnemende sikkel" ) {
      loadCode((clockword) NL_LK_NM, matrix);
    }      
  }
  
}

boolean bepaalZomertijd(int jaar, int maand, int dag) {
  
  // http://home.hccnet.nl/s.f.boukes/html-2/html-263.htm
  // Javascript gedownload en omgebouwd hiernaar

  int i;
  int _weekdag;

  Serial.print("Vandaag ");
  Serial.print(jaar);
  Serial.print("-");
  Serial.print(maand);
  Serial.print("-");
  Serial.print(dag);
  Serial.print(" is dagnummer: ");
  Serial.println(bepaalWeekdag(jaar - 2000, maand, dag));

  boolean _zomertijd = false;

  if (maand <= 2 or maand >= 11) {
    // Jan, Feb, Nov, Dec.
    _zomertijd = false;                                           
  }

  if (maand >= 4 and maand <= 9) {
    // April t.m. september
    _zomertijd = true;                                            
  } else {                                                       

    // Maart of oktober
    if (maand == 3) {
      // Maart                                            
      if (dag < 25) {
        // Begin van de maand is wintertijd                                       
        _zomertijd = false;
      } else {
        for (i = 31; i > 24; i--) {
          // Zoek laatste zondag van de maand, day == 1                              
          if (bepaalWeekdag(jaar - 2000, maand, i) == 1) {
            // Zondag gevonden                    
            Serial.print("Zondag valt op: ");
            Serial.println(i);
            _weekdag = i;
            break;
          }
        }
        if (dag < _weekdag)
          // vergelijk datum                                  
          _zomertijd = false;
        else
          _zomertijd = true;
      }
    }

    if (maand == 10) {
      // Oktober                                           
      if (dag < 25) {
        // Begin van de maand is zomertijd
        _zomertijd = true;                                        
      } else {
        for (i = 31; i > 24; i--) {                              
          // Zoek laatste zondag van de maand, _dagdatum == 1
          if (bepaalWeekdag(jaar - 2000, maand, i) == 1) {                    
            // Zondag gevonden
            Serial.print("Zondag valt op: ");
            Serial.println(i);
            _weekdag = i;
            break;
          }
        }
        if (dag >= _weekdag)
           // vergelijk datum                                 
          _zomertijd = false;
        else
          _zomertijd = true;
      }
    }
  }

  if (_zomertijd) {
    Serial.println("Zomertijd is: true");
  } else {
    Serial.println("Zomertijd is: false");
  }

  return _zomertijd;

}

int bepaalWeekdag(int jaar, int maand, int dag) {
    
  // http://www.wikihow.com/Calculate-the-Day-of-the-Week
  // 1. Add the Day and the value for the Month (from the Month-Table). If the resulting number is greater than 6, subtract the highest multiple of 7 in it. Hold this number till step 3.
  //    tabel: 0,3,3,6,1,4,6,2,5,0,3,5 (jan..dec)
  // 2. Subtract from the (last two digits of the) Year the highest multiple of 28 in it.
  //    Add to the resulting number the number you get when you divide it by 4 and round down (i.e., drop the decimal).
  //    Now add the value for the Century from the Century Table. (dit is 0 voor 2000) (Is dus niet nodig)
  //    If the Month is Jan. or Feb. and the Year is a leap _jaar (gewoon deelbaar door 4), subtract 1.
  // 3. Add together the results from steps 1 and 2.
  //    If the resulting number is greater than 6, subtract the highest multiple of 7 in it.
  //    Using the resulting number, look up the Day-of-week in the Weekday-Table.
  //    tabel: 1,2,3,4,5,6,7 (zo..za)

  int maandTabel[] =
  {
    0, 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5
  };
  
  int res1;
  int res2;
  int res3;

  // 1
  res1 = dag + maandTabel[maand];
  res1 = res1 % 7;

  res2 = jaar % 28;
  res2 += jaar / 4;

  if (jaar % 4 == 0) {
    if (maand == 1 or maand == 2) res2 -= 1;
  }

  res3 = (res1 + res2) % 7;
  if (res3 == 0) res3 = 7;

  return res3;
}
