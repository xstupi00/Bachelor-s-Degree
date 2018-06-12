package InfixToPostfix;

/**
 * Class contains the implementation of CharStack.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class CharStack {

    /**
     * Array representing the CharStack.
     */
    private char data[];

    /**
     * Index of the head of CharStack.
     */
    private int top;

    /**
     * Creates a new CharStack with the size
     * one hundred elements and set the head
     * of the CharStack on the begin.
     */
    protected CharStack() {
        data = new char[100];
        top = -1;
    }

    /**
     * Push char to head of the CharStack.
     * @param c char to pushed.
     */
    protected void push(char c) {
        data[++top] = c;
    }

    /**
     * Obtains (Pop) char from the top of the CharStack.
     * @return char from the top of CharStack
     */
    protected char pop() {
        if(top < 0)
            return '(';
        return data[top--];
    }

    /**
     * Check the emptiness of the CharStack
     * @return isEmpty (True/False)
     */
    protected boolean empty() {
        if(top == -1)
            return true;
        return false;
    }

    /**
     * Check actual state on the CharStack, only
     * obtains char from the top (not Pop).
     * @return char from the top of CharStack
     */
    protected char peek() {
        if(top < 0)
            return '(';
        return data[top];
    }

    /**
     * Function set the CharStack to Empty.
     */
    protected void clear() {
        top = -1;
    }
}