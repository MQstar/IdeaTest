import java.text.ParseException;
import java.util.Arrays;

/**
 * Created by exinqan on 11/25/2016.
 */
public class Test {

    public static final int NO_EQUAL_SIGN = 0;
    public static final int TWO_EQUAL_SIGN = 1;
    public static final int ONE_EQUAL_SING = 2;

    public static void main(String[] args) {

        //String test1="2742e0288d680f016e8400021ac8007a12023c508a80";
        //String test2="28ce074920";
       // String test1="2742e0288d680a02fa1000086b2001e84808f1422a";
       // String test2="28ce0d2480";
        String test1="2742e0288d680f016e8400021ac8007a12023c508a80";
        String test2="28ce074920";
        byte[] a={1,2,3,4};
        byte[] b= Arrays.copyOfRange(a,0,3);
        for (int i = 0; i < b.length; i++) {
            System.out.println(b[i]);
        }


        try {
            System.out.println(base64(paser(test1)));
            System.out.println(base64(paser(test2)));
            System.out.println(paser(test2));
        } catch (Base64Exception e) {
            e.printStackTrace();
        }


    }
    public static String paser(String input){
        String output="";
        for (int i = 0; i < input.length(); i++) {
            switch (input.charAt(i)){
                case '0':
                    output+="0000";
                    break;
                case '1':
                    output+="0001";
                    break;
                case '2':
                    output+="0010";
                    break;
                case '3':
                    output+="0011";
                    break;
                case '4':
                    output+="0100";
                    break;
                case '5':
                    output+="0101";
                    break;
                case '6':
                    output+="0110";
                    break;
                case '7':
                    output+="0111";
                    break;
                case '8':
                    output+="1000";
                    break;
                case '9':
                    output+="1001";
                    break;
                case 'a':
                case 'A':
                    output+="1010";
                    break;
                case 'b':
                case 'B':
                    output+="1011";
                    break;
                case 'c':
                case 'C':
                    output+="1100";
                    break;
                case 'd':
                case 'D':
                    output+="1101";
                    break;
                case 'e':
                case 'E':
                    output+="1110";
                    break;
                case 'f':
                case 'F':
                    output+="1111";
                    break;
                default:
                    break;

            }
        }
        return output;
    }
    public static String base64(String input) throws Base64Exception {



        int type;
        switch (input.length()%6){
            case 0:
                type= NO_EQUAL_SIGN;
                break;
            case 2:
                input+="0000";
                type= TWO_EQUAL_SIGN;
                break;
            case 4:
                input+="00";
                type= ONE_EQUAL_SING;
                break;
            default:
                throw new Base64Exception("length of input is wrong");
        }
        String temp="";
        String output="";
        for (int i = 0; i < input.length(); i++) {
            temp+=input.charAt(i);
            if (temp.length()==6){
                switch (temp){
                    case "000000":
                        output+='A';
                        temp="";
                        break;
                    case "000001":
                        output+='B';
                        temp="";
                        break;
                    case "000010":
                        output+='C';
                        temp="";
                        break;
                    case "000011":
                        output+='D';
                        temp="";
                        break;
                    case "000100":
                        output+='E';
                        temp="";
                        break;
                    case "000101":
                        output+='F';
                        temp="";
                        break;
                    case "000110":
                        output+='G';
                        temp="";
                        break;
                    case "000111":
                        output+='H';
                        temp="";
                        break;
                    case "001000":
                        output+='I';
                        temp="";
                        break;
                    case "001001":
                        output+='J';
                        temp="";
                        break;
                    case "001010":
                        output+='K';
                        temp="";
                        break;
                    case "001011":
                        output+='L';
                        temp="";
                        break;
                    case "001100":
                        output+='M';
                        temp="";
                        break;
                    case "001101":
                        output+='N';
                        temp="";
                        break;
                    case "001110":
                        output+='O';
                        temp="";
                        break;
                    case "001111":
                        output+='P';
                        temp="";
                        break;
                    case "010000":
                        output+='Q';
                        temp="";
                        break;
                    case "010001":
                        output+='R';
                        temp="";
                        break;
                    case "010010":
                        output+='S';
                        temp="";
                        break;
                    case "010011":
                        output+='T';
                        temp="";
                        break;
                    case "010100":
                        output+='U';
                        temp="";
                        break;
                    case "010101":
                        output+='V';
                        temp="";
                        break;
                    case "010110":
                        output+='W';
                        temp="";
                        break;
                    case "010111":
                        output+='X';
                        temp="";
                        break;
                    case "011000":
                        output+='Y';
                        temp="";
                        break;
                    case "011001":
                        output+='Z';
                        temp="";
                        break;
                    case "011010":
                        output+='a';
                        temp="";
                        break;
                    case "011011":
                        output+='b';
                        temp="";
                        break;
                    case "011100":
                        output+='c';
                        temp="";
                        break;
                    case "011101":
                        output+='d';
                        temp="";
                        break;
                    case "011110":
                        output+='e';
                        temp="";
                        break;
                    case "011111":
                        output+='f';
                        temp="";
                        break;
                    case "100000":
                        output+='g';
                        temp="";
                        break;
                    case "100001":
                        output+='h';
                        temp="";
                        break;
                    case "100010":
                        output+='i';
                        temp="";
                        break;
                    case "100011":
                        output+='j';
                        temp="";
                        break;
                    case "100100":
                        output+='k';
                        temp="";
                        break;
                    case "100101":
                        output+='l';
                        temp="";
                        break;
                    case "100110":
                        output+='m';
                        temp="";
                        break;
                    case "100111":
                        output+='n';
                        temp="";
                        break;
                    case "101000":
                        output+='o';
                        temp="";
                        break;
                    case "101001":
                        output+='p';
                        temp="";
                        break;
                    case "101010":
                        output+='q';
                        temp="";
                        break;
                    case "101011":
                        output+='r';
                        temp="";
                        break;
                    case "101100":
                        output+='s';
                        temp="";
                        break;
                    case "101101":
                        output+='t';
                        temp="";
                        break;
                    case "101110":
                        output+='u';
                        temp="";
                        break;
                    case "101111":
                        output+='v';
                        temp="";
                        break;
                    case "110000":
                        output+='w';
                        temp="";
                        break;
                    case "110001":
                        output+='x';
                        temp="";
                        break;
                    case "110010":
                        output+='y';
                        temp="";
                        break;
                    case "110011":
                        output+='z';
                        temp="";
                        break;
                    case "110100":
                        output+='0';
                        temp="";
                        break;
                    case "110101":
                        output+='1';
                        temp="";
                        break;
                    case "110110":
                        output+='2';
                        temp="";
                        break;
                    case "110111":
                        output+='3';
                        temp="";
                        break;
                    case "111000":
                        output+='4';
                        temp="";
                        break;
                    case "111001":
                        output+='5';
                        temp="";
                        break;
                    case "111010":
                        output+='6';
                        temp="";
                        break;
                    case "111011":
                        output+='7';
                        temp="";
                        break;
                    case "111100":
                        output+='8';
                        temp="";
                        break;
                    case "111101":
                        output+='9';
                        temp="";
                        break;
                    case "111110":
                        output+='+';
                        temp="";
                        break;
                    case "111111":
                        output+='/';
                        temp="";
                        break;
                    default:
                        throw new Base64Exception("input is not binary");
                }
            }
        }
        switch (type){
            case NO_EQUAL_SIGN:
                break;
            case ONE_EQUAL_SING:
                output+="=";
                break;
            case TWO_EQUAL_SIGN:
                output+="==";
                break;
            default:
                throw new Base64Exception("no such type");
        }
        return output;
    }
    public static String readVlc(String self) {
        // "Read an unsigned VLC"
        int M = -1;
        char x = '0';
        String INFO = "";
        while (x == '0') {
            x = self.charAt(M + 1);
            M++;
        }
        if (M > 0){
        INFO = self.substring(0,2*M+1);
        }else {
            INFO = "1";
        }
        return INFO;
    }
    /*public static String readSvlc(String self) {
        // "Read a signed VLC"
        String num = readVlc(self);
        if (num.charAt(num.length()-1) == '0')
        num = -num / 2
            else:
        num = (num + 1) / 2
        return num
    }*/
}
