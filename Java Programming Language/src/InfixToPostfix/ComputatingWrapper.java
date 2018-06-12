package InfixToPostfix;


/**
 * Class contains the implementation of Wrapper for
 * computation the Scheme and its Blocks.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public interface ComputatingWrapper {

    /**
     * Declaration of method, which set formula in the
     * Infix type to the relevant variable.
     * @param infix Formula to sets
     * @exception Exception On error format.
     */
    void setInfix(String infix) throws Exception;

    /**
     * Declaration of method, which realizate the calculating the formulae.
     * The method obtains from the temporary Stack two variables and sign
     * for computation between it.
     * @param x Temporary variable to recognized individually factors.
     * @return Result of the computation.
     */
    double getValue(double x);

    /**
     * Declaration of the method, which execute obtains the Infix
     * form of the actual formula.
     * @return String, which contains the Infix form of formula
     */
    String getInfix();

    /**
     * Declaration of the method, which execute the transformation
     * and all the needed operations to obtains the Postfix form of the
     * actual formula.
     * @return String, which contains the Postfix form of formula
     */
    String getPostfix();
}