package scheme;

import unique.UniqueId;

import java.util.Vector;

/**
 * Class contains the implementation of Blocks, which are part of Schemes.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class Block {

    /**
     * Vector of inpurt ports in the Block.
     */
    public Vector<Port> inputPorts = new Vector();

    /**
     * Vector of output ports in the Block.
     */
    public Vector<Port> outputPorts = new Vector();

    /**
     * Vector of Formulae in the Block.
     */
    public Vector<Formula> formulae = new Vector();

    /**
     * Unique ID of the Block.
     */
    public UniqueId ID;

    /**
     * Flag to recognition computation of Block.
     */
    public boolean isComputed = false;

    /**
     * Add Port to the Block, accoording to its type (input/output).
     * Sets the all needed attributes of the Block.
     * @param newPort Port for store.
     */
    public void addPort(Port newPort) {
        if (inputPorts.contains(newPort))
            inputPorts.remove(newPort);
        if (newPort.type == 0)
            inputPorts.add(newPort);
        else if(newPort.type == 1)
            outputPorts.add(newPort);
        newPort.blockID = this.ID;
        newPort.ID = Scheme.portGenerator.genNew("port");
    }

    /**
     * Add Formula to the Block, formula can be overriden by the
     * newest formula.
     * @param newFormula Formula for store.
     */
    public void addFormula(Formula newFormula) {
        if (formulae.contains(newFormula))
            formulae.remove(newFormula);
        formulae.add(newFormula);
    }

    /**
     * Remove port from the Block, from the relevant Vector of Ports,
     * according to its type (input/output).
     * @param toRemove Port to remove.
     */
    public void removePort(Port toRemove)
    {
        if (toRemove.type == 0)
            inputPorts.remove(toRemove);
        else if(toRemove.type == 1)
            outputPorts.remove(toRemove);
    }
}
