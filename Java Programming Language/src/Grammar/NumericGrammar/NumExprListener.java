package Grammar.NumericGrammar;

/**
 * Generated from src/Grammar/GeneralGrammar/Expr.g4 by ANTLR 4.5.1
 */
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link NumExprParser}.
 */
public interface NumExprListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link NumExprParser#expr}.
	 * @param ctx the parse tree
	 */
	void enterExpr(NumExprParser.ExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link NumExprParser#expr}.
	 * @param ctx the parse tree
	 */
	void exitExpr(NumExprParser.ExprContext ctx);
}