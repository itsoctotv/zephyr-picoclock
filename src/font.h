#ifndef FONT_H
#define FONT_H

//FONT[1][2]: 
//      1 = total elements
//      2 = lines inside elements
const unsigned char FONT[128][7] = {
    //values in binary because easier to read
    //ascii values, empty if none used
    { /* ascii_0 */ },
    { /* ascii_1 */ },
    { /* ascii_2 */ },
    { /* ascii_3 */ },
    { /* ascii_4 */ },
    { /* ascii_5 */ },
    { /* ascii_6 */ },
    { /* ascii_7 */ },
    { /* ascii_8 */ },
    { /* ascii_9 */ },
    { /* ascii_10 */ },
    { /* ascii_11 */ },
    { /* ascii_12 */ },
    { /* ascii_13 */ },
    { /* ascii_14 */ },
    { /* ascii_15 */ },
    { /* ascii_16 */ },
    { /* ascii_17 */ },
    { /* ascii_18 */ },
    { /* ascii_19 */ },
    { /* ascii_20 */ },
    { /* ascii_21 */ },
    { /* ascii_22 */ },
    { /* ascii_23 */ },
    { /* ascii_24 */ },
    { /* ascii_25 */ },
    { /* ascii_26 */ },
    { /* ascii_27 */ },
    { /* ascii_28 */ },
    { /* ascii_29 */ },
    { /* ascii_30 */ },
    { /* ascii_31 */ },
    { /* ascii_32 */ },
    { /* ascii_33 */ },
    { /* ascii_34 */ },
    { /* ascii_35 */ },
    { /* ascii_36 */ },
    { /* ascii_37 */ },
    { /* ascii_38 */ },
    { /* ascii_39 */ },
    { /* ascii_40 */ },
    { /* ascii_41 */ },
    { /* ascii_42 */ },
    { /* ascii_43 */ },
    { /* ascii_44 */ },
    { /* ascii_45 */ },
    { /* acsii_46, hex:  0x2e, char: '.' */ 
        
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000110,
        0b00000110
    },
    { /* ascii_47 */ },
    { /* acsii_48, hex: 0x30, char: '0' */ 
        0b00000110,
        0b00001001,
        0b00001001,
        0b00001001,
        0b00001001,
        0b00001001,
        0b00000110
    },
    { /* acsii_49, hex: 0x31, char: '1' */ 
        0b00000010,
        0b00000110,
        0b00001010,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010
    },
    { /* acsii_50, hex: 0x32, char: '2' */ 
        0b00000110,
        0b00001001,
        0b00000001,
        0b00000010,
        0b00000100,
        0b00001000,
        0b00001111
    },
    { /* acsii_51, hex: 0x33, char: '3' */
        0b00000110,
        0b00001001,
        0b00000001,
        0b00000110,
        0b00000001,
        0b00001001,
        0b00000110
    },
    { /* acsii_52, hex: 0x34, char: '4' */ 
        0b00000001,
        0b00000011,
        0b00000101,
        0b00001001,
        0b00001111,
        0b00000001,
        0b00000001
    },
    { /* acsii_53, hex: 0x35, char: '5' */ 
        0b00001111,
        0b00001000,
        0b00001110,
        0b00000001,
        0b00000001,
        0b00001001,
        0b00000110
    },
    { /* acsii_54, hex: 0x36, char: '6' */ 
        0b00000010,
        0b00000100,
        0b00001000,
        0b00001110,
        0b00001001,
        0b00001001,
        0b00000110
    },
    { /* acsii_55, hex: 0x37, char: '7' */ 
        0b00001111,
        0b00001001,
        0b00000001,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010
        
    },
    { /* acsii_56, hex: 0x38, char: '8' */ 
        0b00000110,
        0b00001001,
        0b00001001,
        0b00000110,
        0b00001001,
        0b00001001,
        0b00000110
    },
    { /* acsii_57, hex: 0x39, char: '9' */ 
        0b00000110,
        0b00001001,
        0b00001001,
        0b00000111,
        0b00000001,
        0b00000010,
        0b00000100
    },
    {  /* acsii_58, hex: 0x3a, char: ':' */ 
        0b00000000,         //........
        0b00000110,         //.....11.
        0b00000110,         //.....11.
        0b00000000,         //........
        0b00000110,         //.....11.
        0b00000110,         //.....11.
        0b00000000          //........
    },
    { /* ascii_59 */ },
    { /* ascii_60 */ },
    { /* ascii_61 */ },
    { /* ascii_62 */ },
    { /* ascii_63 */ },
    { /* ascii_64 */ },
    { /* acsii_65, hex: 0x41, char: 'A' */
        0b00000110,         //.....11.
        0b00001001,         //....1..1
        0b00001001,         //....1..1
        0b00001111,         //....1111
        0b00001001,         //....1..1
        0b00001001,         //....1..1
        0b00001001 
    },
    { /* acsii_66, hex: 0x42, char: 'B' */ 
        0b00001110,       //....111.
        0b00001001,       //....1..1
        0b00001001,       //....1..1
        0b00001110,       //....111.
        0b00001001,       //....1..1
        0b00001001,       //....1..1
        0b00001110        //....111. 
    },
    { /* acsii_67, hex: 0x43, char: 'C' */ 
        0b00000110,       //.....11.
        0b00001001,       //....1..1
        0b00001000,       //....1...
        0b00001000,       //....1...
        0b00001000,       //....1...
        0b00001001,       //....1..1
        0b00000110        //.....11. 
    },
    { /* acsii_68, hex: 0x44, char: 'D' */ 
        0b00001110,     //....111.
        0b00001001,     //....1..1
        0b00001001,     //....1..1
        0b00001001,     //....1..1
        0b00001001,     //....1..1
        0b00001001,     //....1..1
        0b00001110      //....111. 
    },
    { /* acsii_69, hex: 0x45, char: 'E' */ 
        0b00001111,     //....1111
        0b00001000,     //....1...
        0b00001000,     //....1...
        0b00001111,     //....1111
        0b00001000,     //....1...
        0b00001000,     //....1...
        0b00001111      //....1111
    },
    { /* acsii_70, hex: 0x46, char: 'F' */ 
        0b00001111,     //....1111
        0b00001000,     //....1...
        0b00001000,     //....1...
        0b00001111,     //....1111
        0b00001000,     //....1...
        0b00001000,     //....1...
        0b00001000      //....1... 
    },
    { /* ascii_71 */ },
    { /* ascii_72 */ },
    { /* ascii_73 */ },
    { /* ascii_74 */ },
    { /* ascii_75 */ },
    { /* ascii_76 */ },
    { /* ascii_77 */ },
    { /* ascii_78 */ },
    { /* ascii_79 */ },
    { /* ascii_80 */ },
    { /* ascii_81 */ },
    { /* ascii_82 */ },
    { /* ascii_83 */ },
    { /* ascii_84 */ },
    { /* ascii_85 */ },
    { /* ascii_86 */ },
    { /* ascii_87 */ },
    { /* ascii_88 */ },
    { /* ascii_89 */ },
    { /* ascii_90 */ },
    { /* ascii_91 */ },
    { /* ascii_92 */ },
    { /* ascii_93 */ },
    { /* ascii_94 */ },
    { /* ascii_95 */ },
    { /* ascii_96 */ },
    { /* ascii_97 */ },
    { /* ascii_98 */ },
    { /* ascii_99 */ },
    { /* ascii_100 */ },
    { /* ascii_101 */ },
    { /* ascii_102 */ },
    { /* ascii_103 */ },
    { /* ascii_104 */ },
    { /* ascii_105 */ },
    { /* ascii_106 */ },
    { /* ascii_107 */ },
    { /* ascii_108 */ },
    { /* ascii_109 */ },
    { /* ascii_110 */ },
    { /* ascii_111 */ },
    { /* ascii_112 */ },
    { /* ascii_113 */ },
    { /* ascii_114 */ },
    { /* ascii_115 */ },
    { /* ascii_116 */ },
    { /* ascii_117 */ },
    { /* ascii_118 */ },
    { /* ascii_119 */ },
    { /* ascii_120 */ },
    { /* ascii_121 */ },
    { /* ascii_122 */ },
    { /* ascii_123 */ },
    { /* ascii_124 */ },
    { /* ascii_125 */ },
    { /* ascii_126 */ },
    { /* ascii_127 */ }
    
};
#endif
