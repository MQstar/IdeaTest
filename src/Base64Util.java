import java.util.Base64;

/**
 * Created by exinqan on 12/9/2016.
 */
public class Base64Util {
    public static byte[] decodeBase64(String str) {
        return Base64.getDecoder().decode(str);
    }

    public static String bytes2String(byte[] data) {
        if(data == null) {
            return null;
        }

        String str = "";

        boolean first  = true;
        for(byte c : data) {
            if(first) {
                str += String.format("0x%02x, ", c);
            }
            else {
                str += String.format(", 0x%02x, ", c);
            }

        }

        return str;
    }
}