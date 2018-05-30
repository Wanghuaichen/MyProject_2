// ----------------------------------------------------------------------------- 
//                                                                            
// (c) Copyright NEDAP N.V. GROENLO HOLLAND                                   
//     ALL RIGHTS RESERVED                                                    
//                                                                            
// -----------------------------------------------------------------------------

#ifndef SYSTEM_INCLUDED
#define SYSTEM_INCLUDED 1

// declaration of IOchannels of ADconvertor ====================================
#define CHANNEL1  0x00
#define CHANNEL2  0x01
#define CHANNEL3  0x02
#define CHANNEL4  0x03
#define CHANNEL5  0x04
#define CHANNEL6  0x05
#define CHANNEL7  0x06
#define CHANNEL8  0x07

// =============================================================================
unsigned char ADConv1Of8( unsigned char Channel);
unsigned char ADConv( unsigned char Channel);

#endif

