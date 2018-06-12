package InfixToPostfix;

/**
 * Class contains the implementation of whole logic needed
 * to computating the mathematically expressions. It contains
 * the methods, which doing transform from the Infix to Postfix
 * form of the individually expressions. Subsequently, other modules
 * executing the calculation the obtained Postfix Expression.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class ComputationRealizator implements ComputatingWrapper {

    /**
     * Stack for the store the Double values at executing the computation.
     */
    private DoubleStack memory;

    /**
     * Stack for the store the char at executing the computation.
     */
    private CharStack operators;

    /**
     * String contains the Postfix form of the actual Expression or is empty.
     */
    private String postfix;

    /**
     * Temporary array for storing the double values at executing the computation.
     */
    private double numbers[];

    /**
     * Stack contains the Infix form of the actual Expression.
     */
    private String infix;

    /**
     * Index in the Infix Expression to knowledge to transformation.
     */
    private int i = 0;

    /**
     * Flag of presence the brackets in the last step.
     */
    private boolean bracketFlag = false;

    /**
     * Actual proccesed char, which will be transform from Infix Expression.
     */
    private char ch;


    /**
     * Initializing and creating the needed attributes of this Class.
     * Creates a new CharStack, DoubleStack, temporary array
     * of double numbers and initializing the Infix Expression.
     */
    public ComputationRealizator() {
        memory = new DoubleStack();
        operators = new CharStack();
        numbers = new double[30];
        infix = "";
    }

    /**
     * According to element on the top of operators stack and actual token, respectively according to their
     * intersection in precedence table performs appropriate one operation. In the case that operators stack
     * is empty, then actual token will be add to the operators stack. As well as in the case that element
     * on the top operators stack was left rounded bracket, or on the intersection in the table is located
     * sign of shift to the stack. In the other case will be operators stack empties and relocation to the
     * output stack. This emptying terminates when operators stack will be empty or on the intersetion in
     * the table will not be sign of reducing.
     * @param x Temporary double variable to execution of computation
     * @return The result of the computation.
     */
    public double getValue(double x) {
        int i = 0;
        memory.clear();
        for(int n = 0; n < postfix.length(); n++) {
            char ch = postfix.charAt(n);
            switch(ch) {
                case '#':
                    memory.push(numbers[i++]);
                    break;
                case 'x':
                    memory.push(x);
                    break;
                case '+':
                    double b = memory.pop();
                    double a = memory.pop();
                    memory.push(a+b);
                    break;
                case '$':
                    memory.push(-memory.pop());
                    break;
                case '-':
                    b = memory.pop();
                    a = memory.pop();
                    memory.push(a-b);
                    break;
                case '*':
                    b = memory.pop();
                    a = memory.pop();
                    memory.push(a*b);
                    break;
                case '/':
                    b = memory.pop();
                    a = memory.pop();
                    memory.push(a/b);
                    break;
                case '^':
                    b = memory.pop();
                    a = memory.pop();
                    memory.push(Math.pow(a,b));
                    break;
                case 'a':
                    memory.push(Math.abs(memory.pop()));
                    break;
                case 'r':
                    memory.push(Math.sqrt(memory.pop()));
                    break;
                case 'e':
                    memory.push(Math.exp(memory.pop()));
                    break;
                case 'l':
                    memory.push(Math.log(memory.pop()));
                    break;
                case 's':
                    memory.push(Math.sin(memory.pop()));
                    break;
                case 'c':
                    memory.push(Math.cos(memory.pop()));
                    break;
            }
        }
        return memory.pop();
    }

    /**
     * Secures selection appropriate one activity according the actual obtaining token.
     * Kind of activity is selected on the basis of resolution of the species
     * operands, operators and left or right rounded bracket. The result of
     * sequence of these operations is eventual postfix expression.
     * In conclusion underway syntax and semantic control some aspects acceptable expressio
     * @exception Exception On error format.
     */
    private String infixToPostfix() throws Exception {
        int n = 0;
        postfix = "";
        infix += " ";
        operators.clear();
        boolean firstTime = true;
        for(;i < infix.length(); i++) {
            ch = infix.charAt(i);
            if(ch == '-' && firstTime)
                ch = '$';
            if(ch != ' ')
                firstTime = false;
            if(isADigit(ch)) {
                int m = i+1;
                while(isADigit(infix.charAt(m)))
                    m++;
                numbers[n++] = convert(infix.substring(i,m));
                postfix += '#';
                i = m-1;
            }

            else if(ch == 'x')
                postfix += ch;
            else if("+-*/^%$".indexOf(ch) >= 0) {
                while(leftFirst(operators.peek(),ch))
                    postfix += operators.pop();
                operators.push(ch);
                bracketFlag = true;
            }
            else if("arelsc".indexOf(ch) >= 0) {
                operators.push('(');
                operators.push(ch);
                i++;
                firstTime = true;
            }
            else if(ch == '(') {
                if (bracketFlag || i == 0) {
                    operators.push(ch);
                    firstTime = true;
                }
                else {
                    throw new Exception("Error format of the given formula!\nBefore the open (left) bracket must be the operator!");
                }
            }
            else if(ch == ')') {
                executeRightBracket();
            }
            else if (Character.isLetter(ch)) {
                throw new Exception("Error format of the given formula!\nUnauthorized occurrence of the char in the postfix expression!");
            }
            if("+-*/^%$".indexOf(ch) < 0)
                bracketFlag = false;
        }
        while(!operators.empty())
            postfix += operators.pop();
        return postfix;
    }

    /**
     * Check the acceptable combinations operators and operands, when the
     * rightBracket has occurred in the Expressions.
     * @exception Exception On error format.
     */
    private void executeRightBracket() throws Exception {
        while(operators.peek() != '(')
            postfix += operators.pop();
        operators.pop();
        if (i < infix.length()-2)
            i++;
        else
            return;
        ch = infix.charAt(i);
        if("+-*/^%$".indexOf(ch) >= 0) {
            while (leftFirst(operators.peek(), ch))
                postfix += operators.pop();
            operators.push(ch);
            bracketFlag = true;
        }
        else if (ch == ')') {
            executeRightBracket();
        }
        else {
            throw new Exception("Error format of the formula!\nUnexpected character in the expression!");
        }
    }

    /**
     * Method recognized the operator at power operation, in compare with
     * others operator in the CharStack and executing the relevant actions.
     * @param a First operator to control
     * @param b Second operator to control.
     * @return Boolean decision about the priority of operators.
     */
    private boolean leftFirst(char a, char b) {
        if(a == '^' && b == '^')
            return false;
        int r = rank(a);
        int s = rank(b);
        return r >= s;
    }


    /**
     * Method to obtains the priority of operator, which is part
     * of the actual expression.
     * @param ch Operator, which will be analyzed.
     * @return Priority of the operator.
     */
    private int rank(char ch) {
        switch(ch) {
            case'+': case'-':
                return 1;
            case'*': case'/':
                return 2;
            case'$':
                return 3;
            case'^':
                return 4;
            default:
                return 0;
        }
    }

    /**
     * Check the given char is the Number or not.
     * @param ch Char, which will be analyzed.
     * @return isDigit(ch) True/False
     */
    private boolean isADigit(char ch) {
        if((ch >= '0' && ch <= '9') || ch == '.')
            return true;
        return false;
    }

    /**
     * Method, which sets the Infix form of the Expression.
     * Function subsequently call the function to transform expression to
     * the Postfix form.
     * @exception Exception On error format.
     */
    public void setInfix(String in) throws Exception {
        infix = in;
        try {
            postfix = infixToPostfix();
        } catch (Exception e) {
            throw new Exception(e.getMessage());
        }
    }


    /**
     * Method, which return the Infix form of the Expression.
     * @return Expression in the Infix Form.
     */
    public String getInfix() {
        return infix;
    }

    /**
     * Method, which executing the calling other function, which
     * executing the transform of expression to the Postfix form.
     * @return Expression in the Postfix Form.
     */
    public String getPostfix() {
        String result = "";
        int num = 0;
        for (int n=0; n<postfix.length(); n++)
            if (postfix.charAt(n) == '#')
                result += " " + numbers[num++];
            else
                result += " " + postfix.charAt(n);
        if (result.length() > 0)
            result = result.substring(1);
        return result;
    }


    /**
     * Execute the convert from the String form to the Double form.
     * @param s String to conversion.
     * @return Tranformed double value from String
     * @exception NumberFormatException On format error.
     * @see NumberFormatException
     */
    private static double convert(String s) {
        try {
            return (new Double(s)).doubleValue();
        }
        catch (NumberFormatException nfe) {
            throw new NumberFormatException("Error to converting the string to the number!");
        }
    }
}