
// =============================================================================
//  procedure : ascbin
//  functie   : Deze procedure is de inverse aan ASCBII
//
//  input     : char *pointer.
//                pointer naar array waar de ascii waarden instaan die
//                geconverteerd moeten
//                worden. De eerste ascii waarde is msb, de laaste lsb.
//            : char nofbytes. Bereik (1 - 5)
//                lengte van string
//
//  output    : short. Bereik 0 - 32767
//               geconverteerde waarde van ascii string of
//               -1 als ascii string andere ascii waarden dan 0-9 bevat
//
// =============================================================================

short ascbin(unsigned char *pointer,short nofbytes)
{
  short byte, total=0;

  for (byte=0; byte < nofbytes;byte++) {
    // als een van de bytes geen getal is dan return FFFF-----------------------
    if (*(pointer+byte) < '0' || *(pointer+byte) > '9') {
      return (0xFFFF);
    }
    total = 10 * total;                         // vorige totaal maal 10
    total = total + *(pointer+byte) - '0';      // vertaal asci naar bin en tel op
  }
  return(total);
}

