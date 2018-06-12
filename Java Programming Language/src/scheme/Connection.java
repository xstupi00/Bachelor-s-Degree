package scheme;

import unique.UniqueId;

import java.util.Iterator;

/**
 * Class contains the implementation of Connection, which are between
 * the two Blocks in the Scheme.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class Connection {

    /**
     * UniqueID of the input port of the connection.
     */
    public UniqueId inputPortID;

    /**
     * UniqueID of the output port of the connection.
     */
    public UniqueId outputPortID;


    /**
     * Create the connection from the two blocks. First port must be
     * input and the second must be set on the output type. The method
     * check the all needed property for creating the connection and
     * subsequently set PairPort of the both Port and set attributes
     * of created Connection.
     * @param inputPort input Port for create the connection
     * @param outputPort output Port for create the connection
     * @throws Exception TypeException on mismatch type
     */
    public void createConnection(Port inputPort, Port outputPort) throws Exception {
        boolean containsFlag = false;
        if (inputPort.type != 0 || outputPort.type != 1) {
            throw new Exception("The mismatch of the type of ports to creating the connection!");
        }
        try {
            for (String y : inputPort.dataSet.keySet()) {
                if (outputPort.dataSet.containsKey(y))
                    containsFlag = true;
            }
        } catch (NullPointerException np) {
            throw new NullPointerException();
        }
        if (containsFlag) {
            inputPort.pairPort = outputPort.ID;
            outputPort.pairPort = inputPort.ID;
            this.inputPortID = inputPort.ID;
            this.outputPortID = outputPort.ID;
        } else {
            throw new Exception("The input port of required connection not contains the common variables with output port!");
        }
    }

    /**
     * Function search the common types between the input and output
     * ports of the connection and store its key and value. Subsequently
     * is returns string, which contains the pairs (key -- value) from the
     * searched types.
     * @param scheme actual scheme
     * @return string with the relevant information about the connection.
     */
    public String viewData(Scheme scheme) {
        String finalString = "";

        Port inputPort = scheme.findPort(this.inputPortID);
        Port outputPort = scheme.findPort(this.outputPortID);
        if (inputPort == null || outputPort == null || inputPort.dataSet == null || outputPort.dataSet == null)
            return "";

        for (String key : inputPort.dataSet.keySet()) {
            if (outputPort.dataSet.containsKey(key))
                finalString = finalString.concat(key + " = " + outputPort.dataSet.get(key).toString() + '\n');
        }
        return finalString;
    }
}