package scheme;

import Grammar.GeneralGrammar.ExprLexer;
import Grammar.GeneralGrammar.ExprParser;
import Grammar.NumericGrammar.NumExprLexer;
import Grammar.NumericGrammar.NumExprParser;
import InfixToPostfix.ComputationRealizator;
import org.antlr.v4.runtime.BailErrorStrategy;
import org.antlr.v4.runtime.ANTLRInputStream;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.misc.ParseCancellationException;
import unique.UniqueId;

/**
 * Class contains the implementation of Formulae, which are the part of the Blocks.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class Formula {

    /**
     * Mathematically expresions of the actual Formula
     */
    public String formula;

    /**
     * UniqueID of block, where the Formula is located
     */
    public UniqueId blockID;

    /**
     * UniqueID of port, where will be stored the result
     */
    public UniqueId portID;

    /**
     * Key from the find relevant Type of the given Port
     * for storing the calculated result
     */
    public String outputKey;

    /**
     * Check, whether the given expression have all needed
     * property for correct mathematically expression.
     * @param exp expression to check
     * @return True if expression is valid, else False
     */
    private boolean isExpr(String exp) {

        ExprLexer lexer = new ExprLexer(new ANTLRInputStream(exp));
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        ExprParser parser = new ExprParser(tokens);

        parser.removeErrorListeners();
        parser.setErrorHandler(new BailErrorStrategy());

        try {
            parser.expr();
            return true;
        } catch (ParseCancellationException e) {
            return false;
        }
    }

    /**
     * Check. whether the given expression contains only
     * numbers (operand and operators) and not contains
     * some other variables or not accepted chars.
     * @param exp expression to check
     * @return True if the expression is valid, else False
     */
    private boolean isNumExpr(String exp) {

        NumExprLexer lexer = new NumExprLexer(new ANTLRInputStream(exp));
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        NumExprParser parser = new NumExprParser(tokens);

        parser.removeErrorListeners();
        parser.setErrorHandler(new BailErrorStrategy());

        try {
            parser.expr();
            return true;
        } catch (ParseCancellationException e) {
            return false;
        }
    }

    /**
     * Set the all attributes of the given Formula
     * @param newFormula String with the mathematically expression
     * @param blockID UniqueID of block where the Formula will be stored
     * @param portID UniquedID of port where the result of formula will be stored
     * @param outKey Key to the HashMap of given Port for stored the result
     * @throws Exception FormulaException
     */
    public void setFormula(String newFormula, UniqueId blockID, UniqueId portID, String outKey) throws Exception {
        if (!isExpr(newFormula)) {
            throw new Exception("Error format of the given formula!");
        } else {
            this.formula = newFormula;
            this.blockID = blockID;
            this.portID = portID;
            this.outputKey = outKey;
        }
    }

    /**
     * Set the given variable to the concret value from the HashMap of given Port.
     * @param varName variable name to replace
     * @param varValue value, which will be set
     * @param gonnaCalc String contains the actual Formula
     * @return Formula with the replace variable
     * @throws Exception FormulaException
     */
    public String setVariable(String varName, double varValue, String gonnaCalc) throws Exception {
        if (!gonnaCalc.contains(varName)) {
            throw new Exception("Error format of the given Formula!\n");
        } else {
            if (varName.equals("a") || varName.equals("r") || varName.equals("e") || varName.equals("l") || varName.equals("s") || varName.equals("c")) {
                for (int i = 0; i < gonnaCalc.length(); i++) {
                    if (gonnaCalc.charAt(i) == varName.charAt(0)) {
                        int tmp = i;
                        if (i+1 < gonnaCalc.length())
                            tmp++;
                        if (gonnaCalc.charAt(tmp) != '(') {
                            gonnaCalc = gonnaCalc.substring(0,i)+String.valueOf(varValue)+gonnaCalc.substring(i+1);
                        }
                    }
                }
            }
            else {
                for (int i = 0; i < gonnaCalc.length(); i++) {
                    if (gonnaCalc.charAt(i) == varName.charAt(0)) {
                        int j = 1;
                        int start = i;
                        for(;i+1 < gonnaCalc.length() && j < varName.length(); j++) {
                            if (gonnaCalc.charAt(++i) != varName.charAt(j))
                                break;
                        }
                        if (j == varName.length())  {
                            if ((i+1 < gonnaCalc.length() && !Character.isLetter(gonnaCalc.charAt(i+1))) || i+1 == gonnaCalc.length()) {
                                if (start < 0)
                                    start = 0;
                                int end = start + varName.length() - 1;
                                if (end > gonnaCalc.length() - 1)
                                    end = gonnaCalc.length() - 1;
                                gonnaCalc = gonnaCalc.substring(0, start) + String.valueOf(varValue) + gonnaCalc.substring(end + 1, gonnaCalc.length());
                            }
                        }
                    }
                }
            }
        }
        return gonnaCalc;
    }

    /**
     * Execution the calculation of actual Formula. Call the all needed
     * methods for reinsurance the correct calculation and execution all
     * required controls.
     * @param scheme actual scheme
     * @return result of the calculation
     * @throws Exception FormulException
     */
    public double calculateFormula(Scheme scheme) throws Exception {
        char ch;
        String gonnaCalc = formula;
        for (int i = 0; i < gonnaCalc.length(); i++) {
            ch = gonnaCalc.charAt(i);
            if (Character.isLetter(ch)) {
                if ( (i+1 < gonnaCalc.length() && gonnaCalc.charAt(i+1) != '(') || i+1 == gonnaCalc.length()) {
                    String variableName = "";
                    variableName += ch;
                    if (i+1 < gonnaCalc.length()) {
                        ch = gonnaCalc.charAt(++i);
                    }
                    else {
                        i++;
                    }
                    while (ch != '+' && ch != '-' && ch != '*' && ch != '/' && ch != '^' && ch != '%' && ch != '$' && ch != ')' && i < gonnaCalc.length()) {
                        variableName = variableName.concat(String.valueOf(ch));
                        if (i+1 < gonnaCalc.length()) {
                            ch = gonnaCalc.charAt(++i);
                        } else {
                            break;
                        }
                    }
                    double value = 0.0;
                    boolean finish = false;
                    for (Port obj : scheme.findBlock(blockID).inputPorts) {
                        if (obj != null) {
                            for (String key : obj.dataSet.keySet()) {
                                if (key.contentEquals(variableName)) {
                                    finish = true;
                                    if (obj.pairPort == null || !scheme.findPort(obj.pairPort).dataSet.containsKey(variableName))
                                        value = obj.dataSet.get(variableName);
                                    else
                                        value = scheme.findPort(obj.pairPort).dataSet.get(variableName);
                                    break;
                                }
                            }
                        }
                        if (finish)
                            break;
                    }
                    try {
                        gonnaCalc = setVariable(variableName, value, gonnaCalc);
                    } catch (Exception e) {
                        throw new Exception(e.getMessage());
                    }
                }
            }
        }

        if (! isNumExpr(gonnaCalc)) {
            System.err.println("The Formula for calculate don't contains the variables!");
            System.exit(1);
        }
        else {
            ComputationRealizator calc_exec = new ComputationRealizator();
            try {
                calc_exec.setInfix(gonnaCalc);
            } catch (Exception e) {
                throw new Exception(e.getMessage());
            }
            return calc_exec.getValue(0);
        }
        return 0;
    }
}