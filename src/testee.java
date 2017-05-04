/**
 * Created by exinqan on 2/8/2017.
 */
public class testee {
    public static final double rate = 0.85;

    public static void main(String[] args) {
        final int MAXIMUM_CAPACITY = 1 << 30;
        int t;
        int n = 2068456789 - 1;
        System.out.println(n);
        t=n >>> 1;
        n |= n >>> 1;
        System.out.println(n + "   " + t);
        t=n >>> 2;
        n |= n >>> 2;System.out.println(n + "   " + t);

        t=n >>> 4;
        n |= n >>> 4;System.out.println(n + "   " + t);

        t=n >>> 8;
        n |= n >>> 8;System.out.println(n + "   " + t);

        t=n >>> 16;
        n |= n >>> 16;
        System.out.println((n < 0) ? 1 : (n >= MAXIMUM_CAPACITY) ? MAXIMUM_CAPACITY : n + 1);
    }
}

