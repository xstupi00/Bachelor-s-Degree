package scheme;

import org.codehaus.jackson.annotate.JsonAutoDetect;
import org.codehaus.jackson.annotate.JsonMethod;
import org.codehaus.jackson.map.ObjectMapper;
import unique.UniqueId;
import unique.UniqueIdCreator;

import java.io.File;
import java.io.IOException;
import java.util.Vector;

import static java.lang.Double.NaN;

/**
 * Class contains the implementation of Scheme, which can containts
 * Blocks and Connections between these Blocks.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class Scheme {

    /**
     * Vector of the Blocks in the Scheme.
     */
    public Vector<Block> blocks = new Vector<>();

    /**
     * Vector of the Connections between the Blocks in the Scheme.
     */
    public Vector<Connection> connections = new Vector<>();

    /**
     * UniqueID of the Scheme
     */
    public UniqueId ID;

    /**
     * Create Generator of UniqueID for the all Blocks
     */
    public UniqueIdCreator blockGenerator = new GenerateID().createGenerator();

    /**
     * Create Generator of UniquedID fot the all Ports
     */
    public static UniqueIdCreator portGenerator = new GenerateID().createGenerator();

    /**
     * Vector of Blocks, which can be calculating in the Scheme
     */
    public Vector<Block> initialBlocks = findInitialBlocks();

    /**
     * Remove the actual Scheme, respectively creating the new
     * empty Vectors of Blocks, Connections and generating new
     * UniqueID for the Scheme.
     */
    public void removeScheme() {
        this.blocks = new Vector<>();
        this.connections = new Vector<>();
        this.ID = new UniqueId();
        this.blockGenerator = new GenerateID().createGenerator();
        //this.portGenerator = null;
    }

    /**
     * Renewal the all Blocks. All port in the Block, will
     * be set the value on the NaN, i.e. non-initialize.
     */
    private void nullBlocks() {
        for (Block block : this.blocks) {
            for (Port port : block.outputPorts) {
                for (String key : port.dataSet.keySet()) {
                    port.updateData(key, NaN);
                }
            }
            for (Port port : block.inputPorts) {
                for (String key : port.dataSet.keySet()) {
                    port.updateData(key, NaN);
                }
            }

        }
    }

    /**
     * Actualization the vector of Scheme after the loading
     * from the external file, which contains the other scheme.
     * At the end of method, is call the method nullBlocks(),
     * because the loaded scheme will be have uninitialized values.
     * @param newScheme Scheme which contains the loaded data from the file.
     */
    private void reloadScheme(Scheme newScheme) {
        this.blocks = newScheme.blocks;
        this.connections = newScheme.connections;
        this.ID = newScheme.ID;
        this.blockGenerator = newScheme.blockGenerator;
        nullBlocks();
        //this.portGenerator = newScheme.portGenerator;
    }


    /**
     * Add block to the Vector of Blocks in the actual Scheme.
     * If the Vector contains this block, will be replace.
     * @param newBlock Block for added.
     */
    public void addBlock(Block newBlock) {
        if (blocks.contains(newBlock))
            blocks.remove(newBlock);
        blocks.add(newBlock);
        newBlock.ID = blockGenerator.genNew("block");
    }

    /**
     * Remove the block from the Vector of Blocks in the actual Scheme.
     * The relevant block will be set to the null, subsequently.
     * @param newBlock Block for remove.
     */
    public void removeBlock(Block newBlock) {
        blocks.remove(newBlock);
        newBlock = null;
    }

    /**
     * Add connection to the Vector of Connection in the actual Scheme.
     * If the Vector contains this connection, will be replace.
     * @param newConnection Connection for added.
     */
    public void addConnection(Connection newConnection) {
        if (connections.contains(newConnection))
            connections.remove(newConnection);
        connections.add(newConnection);
    }

    /**
     * Remove the connection from the Vector of Connections in the actual Scheme.
     * The relevant connection will be set to the null, subsequently.
     * @param newConnection Connection for remove.
     */
    public void removeConnection(Connection newConnection) {
        connections.remove(newConnection);
        newConnection = null;
    }

    /**
     * Method find the blocks, which have the all input ports without connection.
     * @return Block with not connected input ports
     */
    public Vector<Block> findInitialBlocks() {
        Vector<Block> initialBlocks = new Vector<>();
        for (Block actualBlock : this.blocks) {
            boolean flag = true;
            for (Port actualPort : actualBlock.inputPorts) {
                if (actualPort.pairPort != null) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                /*for (Port actualPort : actualBlock.inputPorts) {
                    if (actualPort.pairPort == null) {
                        for (String key : actualPort.dataSet.keySet()) {
                            if (actualPort.dataSet.get(key).equals(NaN))
                                System.out.println("Please insert data: Port " + actualPort.ID + " type with key " + key);
                        }
                    }
                }*/
                if (! initialBlocks.contains((actualBlock)))
                    initialBlocks.add(actualBlock);
            }
        }
        return initialBlocks;
    }

    /**
     * Find block according to the given UniqueID of the specific Block.
     * @param blockID UniquedID of the specific Block
     * @return Block with the same UniqueID if exists, else null.
     */
    public Block findBlock(UniqueId blockID) {
        for (Block block : this.blocks) {
            if (blockID.id() == block.ID.id())
                return block;
        }
        return null;
    }

    /**
     * Find port according to the given UniqueID of the specific Port.
     * @param portID UniqueID of the specific Port
     * @return Port with the same UniqueID if exists, else null.
     */
    public Port findPort(UniqueId portID) {
        for (Block newBlock : this.blocks) {
            for (Port newPort : newBlock.outputPorts) {
                if (newPort.ID.id() == portID.id())
                    return newPort;
            }
            for (Port newPort : newBlock.inputPorts) {
                if (newPort.ID.id() == portID.id())
                    return newPort;
            }
        }
        return null;
    }

    /**
     * Find connection according to the given input and output Ports.
     * @param inputPort input port of the finding connection
     * @param outputPort output port of the finding connection
     * @return Connection contains these two Ports if exist, else false
     */
    public Connection findConnection(Port inputPort, Port outputPort) {
        for (Connection connection : connections) {
            if (connection.inputPortID.id() == inputPort.ID.id() &&
                    connection.outputPortID.id() == outputPort.ID.id())
                return connection;
        }
        return null;
    }

    /**
     * Find Block, which have the some connection with the given Block.
     * @param block Block for which will be looking its connected blocks
     * @return Vector of Blocks, which are connected to the given Block
     */
    public Vector<Block> findConnectedBlocks(Block block) {
        Vector<Block> connectedBlocks = new Vector<>();
        for (Port outPort : block.outputPorts) {
            if (outPort.pairPort != null && !connectedBlocks.contains(findBlock(findPort(outPort.pairPort).blockID))) {
                connectedBlocks.add(findBlock(findPort(outPort.pairPort).blockID));
            }
        }
        return connectedBlocks;
    }

    /**
     * Check the input ports of the given Block. If the port
     * is not connected with the other Port, value will be requested.
     * In the case, when the Port is connected with some other Port,
     * then must be computed Block with this connected Port before it.
     * @param block Block for execution the check
     * @return True if all input port are ready, else false
     */
    public boolean checkInitPorts(Block block) {
        for (Port inPort : block.inputPorts) {
            if (inPort.pairPort != null) {
                for (String key : findPort(inPort.pairPort).dataSet.keySet()) {
                    if (Double.isNaN(findPort(inPort.pairPort).dataSet.get(key)))
                        return false;
                }
            }
        }
        return true;
    }

    /**
     * Sets the flag of computation of the all Blocks in the
     * Scheme to the false. This actions is executing after the
     * finish of each run of computation.
     */
    public void resetComputed()
    {
        for (Block b: blocks)
            b.isComputed = false;
    }


    /**
     * Execution the actions needed to computation
     * the given Port, respectively computation of each formula
     * in this Block.
     * @param block Block for computation.
     * @return UniqueID of computed Block
     * @throws Exception FormulaException
     */
    public int calcblock(Block block) throws Exception
    {
        if (!block.isComputed) {
            for (Formula actualFormula : block.formulae) {
                if (!checkInitPorts(block))
                    break;
                try {
                    findPort(actualFormula.portID).updateData(actualFormula.outputKey, actualFormula.calculateFormula(this));
                } catch (Exception e) {
                    System.out.println(e.getMessage());
                    throw new Exception(e.getMessage());
                }
            }
        }
        if (checkInitPorts(block)) {
            block.isComputed = true;
            Vector<Block> connectedBlocks = findConnectedBlocks(block);
            for (Block newBlock : connectedBlocks)
                if (!this.initialBlocks.contains(newBlock))
                    this.initialBlocks.add(this.initialBlocks.size(), newBlock);
            this.initialBlocks.remove(block);
            if (initialBlocks.size() == 0) {
                resetComputed();
            }
            return block.ID.id();

        } else {
            this.initialBlocks.remove(block);
            if (initialBlocks.size() == 0)
            {
                //throw new Exception("Some error was occurred, try again please.");
                return -1;
            }

            this.initialBlocks.add(this.initialBlocks.size(), block);


        }
        return -1;
    }

    /**
     * Execution the calculation of the all Blocks in the Scheme.
     * @throws Exception FormulaException
     */
    public void computeAllScheme() throws Exception {
        initialBlocks = findInitialBlocks();

        while (!initialBlocks.isEmpty()) {
            calcblock(initialBlocks.get(0));
        }
    }

    /**
     * Detetion Loop betweem the individual Block in the Scheme
     * with using the Transitive Closure.
     * @throws Exception LoopException
     */
    public void detectionLoop() throws Exception {
        Vector<Vector<Integer>> matrix = new Vector<>();

        for (int rows = 0; rows < this.blocks.size(); rows++) {
            Vector<Integer> tmpVector = new Vector<>();
            for (int cols = 0; cols < this.blocks.size(); cols++) {
                tmpVector.add(0);
            }
            matrix.add(tmpVector);
        }

        for (Connection connection : this.connections) {
            int row = this.blocks.indexOf(findBlock(findPort(connection.inputPortID).blockID));
            int column = this.blocks.indexOf(findBlock(findPort(connection.outputPortID).blockID));
            matrix.get(column).set(row, 1);
        }

        if (transitiveClosure(matrix, this.blocks.size()))
            throw new Exception("The scheme contains the loop between the blocks!");
    }

    /**
     * Algorithm for execution the Transitive Closure of the given Matrix.
     * Inspired from: "https://www.geeksforgeeks.org/transitive-closure-of-a-graph/"
     * @param matrix Matrix to Transitive Closure
     * @param size size of the given squared matrix
     * @return true if was find the transitive closure, else false
     */
    private boolean transitiveClosure(Vector<Vector<Integer>> matrix, int size) {
        int reach[][] = new int[size][size];
        int  i, j, k;
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++)
                reach[i][j] = matrix.get(i).get(j);
        for (k = 0; k < size; k++) {
            for (i = 0; i < size; i++) {
                for (j = 0; j < size; j++) {
                    reach[i][j] = (reach[i][j]!=0) ||
                            ((reach[i][k]!=0) && (reach[k][j]!=0))? 1:0;
                }
            }
        }
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++)
                if (i == j && reach[i][j] == 1)
                    return true;
        return false;
    }

    /**
     * Realization of save the Scheme to the given file by user.
     * @param filePath path to save the Scheme
     * @return JSON String with the Scheme
     */
    public String saveScheme(String filePath) {
        ObjectMapper mapperObj = new ObjectMapper();
        mapperObj.setVisibility(JsonMethod.FIELD, JsonAutoDetect.Visibility.ANY);

        String res = "";
        try {
            res = mapperObj.writeValueAsString(this);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return res;
    }


    /**
     * Realization of load the Scheme from the given file by user.
     * @param filePath path to the file, which will be loading
     * @exception IOException On input error.
     * @see IOException
     */
    public void loadScheme(String filePath) throws IOException {
        ObjectMapper mapperObj = new ObjectMapper();
        mapperObj.setVisibility(JsonMethod.FIELD, JsonAutoDetect.Visibility.ANY);

        Scheme newScheme = mapperObj.readValue(filePath, Scheme.class);
        reloadScheme(newScheme);
    }
}
