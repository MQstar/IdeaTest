import java.io.*;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.Properties;

/**
 * Created by exinqan on 4/21/2017.
 */
public class ReflectDemo {
    public static void main(String[] args) {
        try {
            Class c = Class.forName("A");
            Constructor c1=c.getDeclaredConstructor(String.class,String.class);
            c1.setAccessible(true);
            A a=(A)c1.newInstance("src",File.separator);
            Class d = c.getSuperclass();
//            Object sup = d.newInstance();
            Method[] methods = d.getDeclaredMethods();
            for (Method method1 : methods) {
                callMethod(a, method1);
            }
            Method method = c.getDeclaredMethod("add",String.class);
            method.setAccessible(true);
            Object r = method.invoke(a,"abc.properties");
            System.out.println(r);
            propertiesTest((String) r);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void propertiesTest(String propertiesFliePath) throws IOException {
        Properties properties = new Properties();
        InputStream inputStream = new FileInputStream(propertiesFliePath);
        properties.load(inputStream);
        System.out.println(properties.getProperty("abc"));
        System.out.println(properties.getProperty("def"));
        properties.store(new FileOutputStream(propertiesFliePath),"comments");
    }

    private static void callMethod(Object object, Method method) throws IllegalAccessException, InstantiationException, InvocationTargetException {
        method.setAccessible(true);
        method.invoke(object, getMethodParametersInstance(method.getParameters()));
    }
    private static Object[] getMethodParametersInstance(Parameter[] parameters) throws IllegalAccessException, InvocationTargetException, InstantiationException {
        Object[] par = new Object[parameters.length];
        for (int i = 0; i < parameters.length; i++) {
            for (int i1 = 0; i1 < parameters[i].getType().getDeclaredConstructors().length; i1++) {
                parameters[i].getType().getDeclaredConstructors()[i1].setAccessible(true);
                if (parameters[i].getType().getDeclaredConstructors()[i1].getParameters().length == 0){
                    par[i] = parameters[i].getType().getDeclaredConstructors()[i1].newInstance();
                }
            }
        }
        return par;
    }
}
class A extends B{
    private String a;
    private String b;
    private A(String a, String b) {
        this.a = a;
        this.b = b;
    }

    private String  add (String c){
        return a + b + c;
    }
}
class B{
    private void lala(String a, A b){
        System.out.println("!!!!!");
    }
    private void baba(A a,A b){
        System.out.println("?????");
    }
}
