package InfixToPostfix;

/**
 * Class contains the implementation of DoubleStack.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class DoubleStack {

    /**
     * Array representing the DoubleStack.
     */
    private double data[];

    /**
     * Index of the head of DoubleStack.
     */
    private int top;

    /**
     * Creates a new DoubleStack with the size
     * one hundred elements and set the head
     * of the DoubleStack on the begin.
     */
    protected DoubleStack() {
        data = new double[100];
        top = -1;
    }

    /**
     * Push double to head of the DoubleStack.
     * @param c double to pushed.
     */
    protected void push(double c) {
        data[++top] = c;
    }

    /**
     * Obtains (Pop) double from the top of the DoubleStack.
     * @return duouble from the top of DoubleStack
     */
    protected double pop() {
        return data[top--];
    }

    /**
     * Check the emptiness of the DoubleStack
     * @return isEmpty (True/False)
     */
    protected boolean empty() {
        if(top == -1)
            return true;
        return false;
    }

    /**
     * Function set the DoubleStack to Empty.
     */
    protected void clear() {
        top = -1;
    }
}