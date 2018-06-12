package sample;

import javafx.event.EventHandler;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.input.MouseDragEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;
import javafx.stage.FileChooser;

import org.codehaus.jackson.JsonParser;
import org.codehaus.jackson.annotate.JsonAutoDetect;
import org.codehaus.jackson.annotate.JsonMethod;
import org.codehaus.jackson.map.ObjectMapper;
import scheme.Block;
import scheme.Connection;
import scheme.GenerateID;
import scheme.Scheme;
import unique.UniqueIdCreator;

import javax.json.Json;
import javax.json.JsonArrayBuilder;
import javax.json.JsonObjectBuilder;
import java.io.*;
import java.util.HashSet;
import org.json.*;

class GUIScheme {

    Pane m_pane;
    Scheme m_scheme;
    Label m_terminal;
    Boolean m_steps;
    HashSet<GUIBlock> m_blocks;
    HashSet<GUIConnection> m_connections;
    HashSet<GUIBlock> m_unreadyBlocks;
    private GUIController m_controler;
    private String m_saved;
    int m_lastCalced = -1;

    public class DragStartHandler implements EventHandler<MouseEvent> {
        public GUIConnection line;

        @Override
        public void handle(MouseEvent event) {
            //System.out.println("POUZIVAM SA JUPIIII");
            if (line == null) {
                Node sourceNode = (Node) event.getSource();
                line = new GUIConnection();
                line.setStartPort(sourceNode);
                line.setEndX(line.getStartX());
                line.setEndY(line.getStartY());
                sourceNode.startFullDrag();
                m_pane.getChildren().add(0, line);
            }
        }
    }

    //
    public DragStartHandler m_dragStart;
    public EventHandler<MouseEvent> m_dragEnter;
    public EventHandler<MouseDragEvent> m_dragRelease;

    GUIScheme(Pane pane, Label terminal, GUIController controller) {
        m_terminal = terminal;
        m_controler = controller;
        m_blocks = new HashSet<>();
        m_connections = new HashSet<>();
        m_unreadyBlocks = new HashSet<>();
        m_pane = pane;
        UniqueIdCreator schemeGenerator = new GenerateID().createGenerator();
        m_scheme = new Scheme();
        m_scheme.ID = schemeGenerator.genNew("scheme");

//        m_terminal.setText("Scheme no. " + m_scheme.ID.id() + ":");

        ///ciary

        m_dragStart = new DragStartHandler();


        m_dragRelease = evt -> {
            //System.out.println("DRAGRELEASE");

            Node node = (Node) evt.getSource();
            m_dragStart.line.setEndPort(node);
            evt.consume();
            m_dragStart.line = null;
        };

        m_dragEnter = evt -> {
            //System.out.println("DRAGENTER");

            if (m_dragStart.line != null) {
                Node node = (Node) evt.getSource();
                m_dragStart.line.centerIfShould(node);
            }


        };


        m_pane.setOnMouseReleased(evt -> {
            m_pane.getChildren().remove(m_dragStart.line);
            m_dragStart.line = null;
        });

        m_pane.setOnMouseDragged(evt -> {

            if (m_dragStart.line != null) {
                Node pickResult = evt.getPickResult().getIntersectedNode();
                if (pickResult == null || pickResult.getUserData() != Boolean.TRUE) {
                    m_dragStart.line.setEndX(evt.getX());
                    m_dragStart.line.setEndY(evt.getY());
                }
            }
        });
    }

    void addBlock(GUIBlock block) {
        m_scheme.addBlock(block.block());
        m_pane.getChildren().add(block);
        m_blocks.add(block);
    }

    void removeBlock(GUIBlock block) {
        m_pane.getChildren().remove(block);
        m_scheme.removeBlock(block.block());
        m_blocks.remove(block);

        m_unreadyBlocks.remove(block);
    }

    void calcStep() {
        if (m_scheme.initialBlocks.isEmpty())
            m_scheme.initialBlocks = m_scheme.findInitialBlocks();

        int id = -1;

        m_steps = true;

        while (id < 0) {
            try {
                id = m_scheme.calcblock(m_scheme.initialBlocks.get(0));

            }
            catch (Exception e) {
                //e.printStackTrace();
                m_terminal.setText("Some error was occurred at computing!");
                break;
            }
        }

        if (m_lastCalced >= 0)
            getBlockById(m_lastCalced).lightOff();

        getBlockById(id).lightOn();
        getBlockById(id).computingDone();
        m_lastCalced = id;

        if (m_scheme.initialBlocks.isEmpty()) {
            m_controler.stepDone();
        }

    }

    void stepsGetReady()
    {
        try {
            m_scheme.detectionLoop();
        }
        catch (Exception e) {

            m_terminal.setText("LOOP DETECTION: " + e.getMessage());
            return;
        }

        m_steps = true;
        for (GUIBlock block: m_blocks)
            block.getReady();
    }

    void stepsDone()
    {
        if (m_lastCalced >= 0)
            getBlockById(m_lastCalced).lightOff();

        m_lastCalced = -1;
        m_scheme.initialBlocks.clear();
        m_steps = false;
        m_controler.stepGetReady();
        m_scheme.resetComputed();
    }

    public void saveScheme()
    {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Resource File");
        fileChooser.getExtensionFilters().add(
                new FileChooser.ExtensionFilter("Scheme Editor Json", "*.sej"));

        fileChooser.setInitialFileName("unnamed_scheme.sej");
        File selectedFile = fileChooser.showSaveDialog(Main.stage);
        //System.out.println(selectedFile);


        JsonObjectBuilder builder = Json.createObjectBuilder();
        builder.add("scheme", m_scheme.ID.id());

        JsonArrayBuilder blockBuilder = Json.createArrayBuilder();

        JsonArrayBuilder connectionBuilder = Json.createArrayBuilder();

        for (GUIBlock block : m_blocks)
            blockBuilder.add(block.serialize());

        int i = 0;
        for (GUIConnection connection : m_connections) {
            connectionBuilder.add(connection.serialize());
            ++i;
        }

        builder.add("connections", connectionBuilder.build());
        builder.add("blocks", blockBuilder.build());
        builder.add("m_scheme", m_scheme.saveScheme(""));

        m_saved = builder.build().toString();

        try {
            PrintWriter out = new PrintWriter(selectedFile);
            out.print(m_saved);
            out.close();

        }
        catch (Exception e) {
            m_terminal.setText("The save of file was not succesfull.");
            //e.printStackTrace();
        }

    }

    public void resetAll()
    {
        //System.out.println(m_saved);

        HashSet<GUIBlock> tmp = new HashSet<>();
        tmp.addAll(m_blocks);

        for (GUIBlock block : tmp)
            removeBlock(block);


        HashSet<GUIConnection> tmp1 = new HashSet<>();
        tmp1.addAll(m_connections);

        for (GUIConnection connection : tmp1)
            removeConnection(connection);

        m_blocks = new HashSet<>();
        m_connections = new HashSet<>();
        m_unreadyBlocks = new HashSet<>();
        m_scheme.removeScheme();

//        m_terminal.setText("Scheme no. " + m_scheme.ID.id() + ":");
    }

    public void loadScheme() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Resource File");
        fileChooser.getExtensionFilters().add(
                new FileChooser.ExtensionFilter("Scheme Editor Json", "*.sej"));

        File selectedFile = fileChooser.showOpenDialog(Main.stage);
        //System.out.println(selectedFile);

        BufferedReader in;
        String jsonik;
        try {
            in = new BufferedReader(new FileReader(selectedFile));
            jsonik = in.readLine();


        }
        catch (Exception e) {
            m_terminal.setText("The load of file was not succesfull.");
            //e.printStackTrace();
            return;
        }


        resetAll();

        try {

            JSONObject obj = new JSONObject(jsonik);
            String mScheme = obj.getString("m_scheme");
            m_scheme.loadScheme(mScheme);

            JSONArray arrBlocks = obj.getJSONArray("blocks");

            for (int i = 0; i < arrBlocks.length(); i++) {
                loadBlock(arrBlocks.getJSONObject(i));
            }

            JSONArray arrConnections = obj.getJSONArray("connections");

            for (int i = 0; i < arrConnections.length(); i++) {
                loadConnection(arrConnections.getJSONObject(i));
            }
        }
        catch (Exception E) {
            m_terminal.setText("The load of file was not succesfull.");
            //E.printStackTrace();
        }
//        m_terminal.setText("Scheme no. " + m_scheme.ID.id() + ":");
    }

    public void loadBlock(JSONObject block) throws Exception
    {
        GUIBlock tmp = GUIBlock.deserialize(block);
        m_blocks.add(tmp);
        //System.out.println(m_scheme.findBlock(tmp.m_id));
        //System.out.println(tmp.m_id.id());
        //System.out.println(m_scheme);
        tmp.m_block = m_scheme.findBlock(tmp.m_id);
        tmp.pairPorts(m_scheme);
        m_pane.getChildren().add(tmp);

    }

    public void loadConnection(JSONObject connection) throws Exception
    {
        GUIConnection tmp = GUIConnection.deserialize(connection);
        m_connections.add(tmp);
        tmp.m_connection = m_scheme.findConnection(tmp.m_input.port(), tmp.m_output.port());
    }

    void calculate()
    {
        stepsDone();


        //System.out.println(m_scheme.connections);
        //System.out.println(m_scheme.blocks);

        try {
            m_scheme.detectionLoop();
        }
        catch (Exception e) {

            m_terminal.setText("LOOP DETECTION: " + e.getMessage());
            return;
        }

        for (GUIBlock block: m_blocks) {
            //System.out.println("BLOCK UNREADY");
            m_unreadyBlocks.add(block);
        }

        for (GUIBlock block: m_blocks)
            block.getReady();

    }

    void imReady(GUIBlock block)
    {
        //System.out.println("block ready");

        m_unreadyBlocks.remove(block);


        try {
            if (m_unreadyBlocks.isEmpty()) {
                //System.out.println("ale sak to ratam more");

                if (m_steps) {
                    m_controler.stepStep();
                }
                else {
                    m_scheme.resetComputed();
                    m_scheme.computeAllScheme();

                    for (GUIBlock tmp : m_blocks) {
                        tmp.computingDone();
                    }
                }
            }
        }
        catch (Exception e) {
            m_terminal.setText("The calculate of scheme was not succesfull");
        }

        m_terminal.setText("Succesfull calculating!");

    }

    void addConnection(GUIConnection connection)
    {
        m_scheme.addConnection(connection.connection());
        m_connections.add(connection);
    }

    void removeConnection(GUIConnection connection)
    {
        m_scheme.removeConnection(connection.connection());
        m_connections.remove(connection);
        m_pane.getChildren().remove(connection);
    }

    Pane pane()
    {
        return m_pane;
    }

    GUIBlock getBlockById(int id)
    {
        for (GUIBlock block : m_blocks)
            if (block.block().ID.id() == id)
                return block;

        return null;
    }
}
