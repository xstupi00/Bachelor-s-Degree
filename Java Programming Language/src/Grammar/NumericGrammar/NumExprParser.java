package Grammar.NumericGrammar;

/**
 * Generated from src/Grammar/GeneralGrammar/Expr.g4 by ANTLR 4.5.1
 */
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class NumExprParser extends Parser {

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		T__9=10, T__10=11, T__11=12, T__12=13, T__13=14, Const=15, WS=16;
	public static final int
		RULE_expr = 0;
	public static final String[] ruleNames = {
		"expr"
	};

	private static final String[] _LITERAL_NAMES = {
		null, "'-'", "'('", "')'", "'*'", "'+'", "'/'", "'^'", "'%'", "'a'", "'r'", 
		"'e'", "'l'", "'s'", "'c'"
	};
	private static final String[] _SYMBOLIC_NAMES = {
		null, null, null, null, null, null, null, null, null, null, null, null, 
		null, null, null, "Const", "WS"
	};
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "NumExpr.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public NumExprParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}
	public static class ExprContext extends ParserRuleContext {
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode Const() { return getToken(NumExprParser.Const, 0); }
		public ExprContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_expr; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof NumExprListener ) ((NumExprListener)listener).enterExpr(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof NumExprListener ) ((NumExprListener)listener).exitExpr(this);
		}
	}

	public final ExprContext expr() throws RecognitionException {
		return expr(0);
	}

	private ExprContext expr(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		ExprContext _localctx = new ExprContext(_ctx, _parentState);
		ExprContext _prevctx = _localctx;
		int _startState = 0;
		enterRecursionRule(_localctx, 0, RULE_expr, _p);
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(40);
			switch (_input.LA(1)) {
			case T__0:
				{
				setState(3);
				match(T__0);
				setState(4);
				expr(15);
				}
				break;
			case T__1:
				{
				setState(5);
				match(T__1);
				setState(6);
				expr(0);
				setState(7);
				match(T__2);
				}
				break;
			case T__8:
				{
				setState(9);
				match(T__8);
				setState(10);
				match(T__1);
				setState(11);
				expr(0);
				setState(12);
				match(T__2);
				}
				break;
			case T__9:
				{
				setState(14);
				match(T__9);
				setState(15);
				match(T__1);
				setState(16);
				expr(0);
				setState(17);
				match(T__2);
				}
				break;
			case T__10:
				{
				setState(19);
				match(T__10);
				setState(20);
				match(T__1);
				setState(21);
				expr(0);
				setState(22);
				match(T__2);
				}
				break;
			case T__11:
				{
				setState(24);
				match(T__11);
				setState(25);
				match(T__1);
				setState(26);
				expr(0);
				setState(27);
				match(T__2);
				}
				break;
			case T__12:
				{
				setState(29);
				match(T__12);
				setState(30);
				match(T__1);
				setState(31);
				expr(0);
				setState(32);
				match(T__2);
				}
				break;
			case T__13:
				{
				setState(34);
				match(T__13);
				setState(35);
				match(T__1);
				setState(36);
				expr(0);
				setState(37);
				match(T__2);
				}
				break;
			case Const:
				{
				setState(39);
				match(Const);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
			_ctx.stop = _input.LT(-1);
			setState(62);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,2,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(60);
					switch ( getInterpreter().adaptivePredict(_input,1,_ctx) ) {
					case 1:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(42);
						if (!(precpred(_ctx, 13))) throw new FailedPredicateException(this, "precpred(_ctx, 13)");
						setState(43);
						match(T__3);
						setState(44);
						expr(14);
						}
						break;
					case 2:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(45);
						if (!(precpred(_ctx, 12))) throw new FailedPredicateException(this, "precpred(_ctx, 12)");
						setState(46);
						match(T__4);
						setState(47);
						expr(13);
						}
						break;
					case 3:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(48);
						if (!(precpred(_ctx, 11))) throw new FailedPredicateException(this, "precpred(_ctx, 11)");
						setState(49);
						match(T__0);
						setState(50);
						expr(12);
						}
						break;
					case 4:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(51);
						if (!(precpred(_ctx, 10))) throw new FailedPredicateException(this, "precpred(_ctx, 10)");
						setState(52);
						match(T__5);
						setState(53);
						expr(11);
						}
						break;
					case 5:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(54);
						if (!(precpred(_ctx, 9))) throw new FailedPredicateException(this, "precpred(_ctx, 9)");
						setState(55);
						match(T__6);
						setState(56);
						expr(10);
						}
						break;
					case 6:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(57);
						if (!(precpred(_ctx, 8))) throw new FailedPredicateException(this, "precpred(_ctx, 8)");
						setState(58);
						match(T__7);
						setState(59);
						expr(9);
						}
						break;
					}
					} 
				}
				setState(64);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,2,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	public boolean sempred(RuleContext _localctx, int ruleIndex, int predIndex) {
		switch (ruleIndex) {
		case 0:
			return expr_sempred((ExprContext)_localctx, predIndex);
		}
		return true;
	}
	private boolean expr_sempred(ExprContext _localctx, int predIndex) {
		switch (predIndex) {
		case 0:
			return precpred(_ctx, 13);
		case 1:
			return precpred(_ctx, 12);
		case 2:
			return precpred(_ctx, 11);
		case 3:
			return precpred(_ctx, 10);
		case 4:
			return precpred(_ctx, 9);
		case 5:
			return precpred(_ctx, 8);
		}
		return true;
	}

	public static final String _serializedATN =
		"\3\u0430\ud6d1\u8206\uad2d\u4417\uaef1\u8d80\uaadd\3\22D\4\2\t\2\3\2\3"+
		"\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2"+
		"\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3"+
		"\2\3\2\5\2+\n\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3\2\3"+
		"\2\3\2\3\2\3\2\3\2\7\2?\n\2\f\2\16\2B\13\2\3\2\2\3\2\3\2\2\2P\2*\3\2\2"+
		"\2\4\5\b\2\1\2\5\6\7\3\2\2\6+\5\2\2\21\7\b\7\4\2\2\b\t\5\2\2\2\t\n\7\5"+
		"\2\2\n+\3\2\2\2\13\f\7\13\2\2\f\r\7\4\2\2\r\16\5\2\2\2\16\17\7\5\2\2\17"+
		"+\3\2\2\2\20\21\7\f\2\2\21\22\7\4\2\2\22\23\5\2\2\2\23\24\7\5\2\2\24+"+
		"\3\2\2\2\25\26\7\r\2\2\26\27\7\4\2\2\27\30\5\2\2\2\30\31\7\5\2\2\31+\3"+
		"\2\2\2\32\33\7\16\2\2\33\34\7\4\2\2\34\35\5\2\2\2\35\36\7\5\2\2\36+\3"+
		"\2\2\2\37 \7\17\2\2 !\7\4\2\2!\"\5\2\2\2\"#\7\5\2\2#+\3\2\2\2$%\7\20\2"+
		"\2%&\7\4\2\2&\'\5\2\2\2\'(\7\5\2\2(+\3\2\2\2)+\7\21\2\2*\4\3\2\2\2*\7"+
		"\3\2\2\2*\13\3\2\2\2*\20\3\2\2\2*\25\3\2\2\2*\32\3\2\2\2*\37\3\2\2\2*"+
		"$\3\2\2\2*)\3\2\2\2+@\3\2\2\2,-\f\17\2\2-.\7\6\2\2.?\5\2\2\20/\60\f\16"+
		"\2\2\60\61\7\7\2\2\61?\5\2\2\17\62\63\f\r\2\2\63\64\7\3\2\2\64?\5\2\2"+
		"\16\65\66\f\f\2\2\66\67\7\b\2\2\67?\5\2\2\r89\f\13\2\29:\7\t\2\2:?\5\2"+
		"\2\f;<\f\n\2\2<=\7\n\2\2=?\5\2\2\13>,\3\2\2\2>/\3\2\2\2>\62\3\2\2\2>\65"+
		"\3\2\2\2>8\3\2\2\2>;\3\2\2\2?B\3\2\2\2@>\3\2\2\2@A\3\2\2\2A\3\3\2\2\2"+
		"B@\3\2\2\2\5*>@";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}