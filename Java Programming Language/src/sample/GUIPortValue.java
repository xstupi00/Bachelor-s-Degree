package sample;

class GUIPortValue {
    private String name;
    private String m_formula;
    private double value;

    GUIPortValue(String newName)
    {
        name = newName;
        value = Double.NaN;
        m_formula = "";
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public void setFormula(String formula)
    {
        m_formula = formula;
    }

    public void setValue(double value)
    {
        this.value = value;
    }

    public double getValue() {
        return value;
    }

    public String getFormula() {
        return m_formula;
    }

    public String getName() {
        return name;
    }

    public String stringify() {

        if (m_formula.isEmpty())
            return name + ": " + value;

        else
            return name + ": " + value + " ( = " + m_formula + ")";
    }
}
