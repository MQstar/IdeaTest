
/**
 * Created by exinqan on 5/2/2017.
 */
public class RedBlackTree {
    private Node root;
    private int length;

    class Node {
        private Key key;
        private Value value;
        private Node parent;
        private Node leftNode;
        private Node rightNode;
        private boolean red;

        Node(Key key, Value value, Node parent) {
            this.key = key;
            this.value = value;
            this.parent = parent;
        }
    }
    public void put(Key key, Value value){
        if (key == null || value == null){
            return;
        }
        put(root, null, key, value);
    }

    private void put(Node node, Node parent, Key key, Value value){
        if (root == null) {
            root = new Node(key, value, parent);
            this.length++;
            adjust();
        }
        int cmp = root.key.compareTo(key);
        if (cmp > 0){
            put(root.rightNode, root, key, value);
        }else if (cmp < 0){
            put(root.leftNode, root, key, value);
        }else {
            root.value = value;
        }
    }

    private void adjust() {

    }

    public Value get(Key key){
        return get(root, key);
    }

    private Value get(Node root, Key key){
        if (root == null){
            return null;
        }
        int cmp = root.key.compareTo(key);
        if (cmp > 0){
            return get(root.rightNode, key);
        }else if (cmp < 0){
            return get(root.leftNode, key);
        }else {
            return root.value;
        }
    }

    public Node getMin(){
        return getMin(root);
    }

    private Node getMin(Node root){
        if (root == null){
            return null;
        }
        if (root.leftNode !=null){
            return getMin(root.leftNode);
        }else {
            return root;
        }
    }

    public void delete(Key key){
        delete(root, key);
    }

    private void delete(Node root, Key key){
        if (root == null){
            return;
        }
        int cmp = root.key.compareTo(key);
        if (cmp > 0){
            delete(root.rightNode, key);
        }else if (cmp < 0){
            delete(root.leftNode, key);
        }else {
            if (root == this.root){
                root = null;
                return;
            }

        }
    }
}
class Key implements Comparable<Key>{
    @Override
    public int compareTo(Key o) {
        return 0;
    }
}
class Value{

}