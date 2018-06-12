package sample;

import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.geometry.HPos;
import javafx.scene.control.*;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Pane;
import javafx.scene.layout.RowConstraints;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import org.json.JSONObject;
import scheme.*;
import unique.UniqueId;
import unique.UniqueIdCreator;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonObjectBuilder;
import java.util.HashSet;
import java.util.Optional;
import java.util.Vector;


class GUIBlock extends GridPane {

    GUIScheme scheme() {
        return GUIController.m_scheme;
    }

    private void presetName() {
        m_name = new Label("UNNAMED");
        m_name.setFont(Font.font("Verdana", FontWeight.BOLD, 12));
        GridPane.setHalignment(m_name, HPos.CENTER);
        add(m_name, 1, 4);
    }

    private void presetStructure() {
        setMinSize(110, 90);
        setMaxSize(110, 90);
        setPrefSize(110, 90);

        getColumnConstraints().addAll(new ColumnConstraints(20),
                new ColumnConstraints(70),
                new ColumnConstraints(20));

        getRowConstraints().addAll(new RowConstraints(10),
                new RowConstraints(10),
                new RowConstraints(10),
                new RowConstraints(10),
                new RowConstraints(10),
                new RowConstraints(10),
                new RowConstraints(10),
                new RowConstraints(10),
                new RowConstraints(10));
    }

    private void presetPorts() {

        for (int i = 0; i <= 2; i += 2) {
            for (int j = 0; j <= 8; j += 2) {
                GUIPort tmp = new GUIPort(this);
                add(tmp, i, j);
                m_availablePorts.add(tmp);
                m_allPorts.add(tmp);
            }
        }

    }

    private void presetContentMenu() {
        // MENU ITEM - Add input
        MenuItem addInput = new MenuItem("Add input port");
        addInput.setOnAction(t -> {
            makePortsAvailable();
            m_toRegister = GUIPort.Type.input;
        });

        // MENU ITEM - Add output
        MenuItem addOutput = new MenuItem("Add output port");
        addOutput.setOnAction(t -> {
            makePortsAvailable();
            m_toRegister = GUIPort.Type.output;
        });

        MenuItem formula = new MenuItem("Add formula");
        formula.setOnAction(t -> {
            addFormla();
        });

        // MENU ITEM - Delete
        MenuItem delete = new MenuItem("Delete");
        delete.setOnAction(t -> scheme().removeBlock(this));

        // MENU ITEM - Rename
        MenuItem rename = new MenuItem("Rename");
        rename.setOnAction(t -> changeName());

        m_menu = new ContextMenu();

        m_menu.addEventFilter(MouseEvent.MOUSE_RELEASED, event -> {
            if (event.getButton() == MouseButton.SECONDARY) {
                //System.out.println("Right-click on block menu. Consuming event.");
                event.consume();
            }
        });

        m_menu.getItems().addAll(addInput,
                addOutput,
                new SeparatorMenuItem(),
                formula,
                new SeparatorMenuItem(),
                rename,
                new SeparatorMenuItem(),
                delete);

        setOnMouseClicked(e -> {
            if (e.getButton() == MouseButton.SECONDARY) {
                //System.out.println("Right click on block, showing menu");

                m_menu.show(this, e.getScreenX(), e.getScreenY());
            } else {
                //System.out.println("Left click on block, disabling menu if is showing.");

                if (m_menu.isShowing())
                    m_menu.hide();
            }
        });

    }

    private void makeDraggable() {
        setOnMousePressed(evt -> {
            m_mouseX = evt.getSceneX();
            m_mouseY = evt.getSceneY();
        });

        setOnMouseDragged(evt -> {
            double deltaX = evt.getSceneX() - m_mouseX;
            double deltaY = evt.getSceneY() - m_mouseY;

            double toSetX = getLayoutX() + deltaX;
            double toSetY = getLayoutY() + deltaY;

            if (toSetX > scheme().pane().getWidth() - getWidth())
                toSetX = scheme().pane().getWidth() - getWidth();

            if (toSetY > scheme().pane().getHeight() - getHeight())
                toSetY = scheme().pane().getHeight() - getHeight();

            if (toSetX < 0)
                toSetX = 0;

            if (toSetY < 0)
                toSetY = 0;

            setLayoutX(toSetX);
            setLayoutY(toSetY);

            m_mouseX = evt.getSceneX();
            m_mouseY = evt.getSceneY();
        });

        m_onDrag = getOnMouseDragged();
        m_onPress = getOnMousePressed();
    }

    GUIBlock(GUIScheme scheme) {
        super();
        m_block = new Block();

//        scheme;
        m_availablePorts = new HashSet<>();
        m_allPorts = new Vector<>();
        m_outputPorts = new HashSet<>();
        m_unreadyPorts = new HashSet<>();
        m_inputPorts = new HashSet<>();

        setVisible(true);
        setStyle("-fx-background-color: coral");

        presetStructure();
        presetName();
        presetPorts();
        presetContentMenu();
        makeDraggable();
    }

    void makePortsAvailable() {
        for (GUIPort port : m_availablePorts) {
            port.appendAvailable();
        }
    }

    void registerUsed(GUIPort port) {
        m_availablePorts.remove(port);
        switch (m_toRegister) {
            case input:
                port.setAsIn();
                m_inputPorts.add(port);
                break;

            case output:
                port.setAsOut();
                m_outputPorts.add(port);
                break;
        }

        for (GUIPort tmp : m_availablePorts) {
            tmp.disappear();
        }

        m_block.addPort(port.port());

    }

    void registerUsed(GUIPort port, GUIPort.Type type) {
        switch (type) {
            case input:
                m_inputPorts.add(port);
                port.setAsIn();
                m_availablePorts.remove(port);
                break;

            case output:
                m_outputPorts.add(port);
                port.setAsOut();
                m_availablePorts.remove(port);
                break;
        }
    }

    void registerUnused(GUIPort port) {
        m_block.removePort(port.port());
        port.reset();

        if (port.type() == GUIPort.Type.output)
            m_outputPorts.remove(port);

        if (port.type() == GUIPort.Type.output)
            m_inputPorts.remove(port);

        if (m_unreadyPorts.contains(port))
            m_unreadyPorts.remove(port);

        m_availablePorts.add(port);
    }

    void changeName() {
        TextInputDialog dialog = new TextInputDialog(m_name.getText());
        dialog.setTitle("Change block name");
        dialog.setHeaderText(null);
        dialog.setGraphic(null);
        dialog.setContentText("New name:");
        Optional<String> result = dialog.showAndWait();
        result.ifPresent(s -> m_name.setText(s));

    }

    void disableDragging() {
        //System.out.println("Block mouse dragging disabled");
        setOnMouseDragged(null);
        setOnMousePressed(null);
    }

    void enableDragging() {
        //System.out.println("Block mouse dragging enabled");
        setOnMouseDragged(m_onDrag);
        setOnMousePressed(m_onPress);
    }

    Block block() {
        return m_block;
    }

    void addFormla() {
        for (GUIPort port : m_outputPorts) {
            port.makeFormulaAvailable();
        }
    }

    void registerFormulaNoDisable(GUIPort port, String key) {
        Formula formula = new Formula();

        TextInputDialog dialog = new TextInputDialog(m_name.getText());
        dialog.setTitle("Enter formula");
        dialog.setHeaderText(null);
        dialog.setGraphic(null);
        dialog.setContentText("formula for " + key + " :");
        Optional<String> result = dialog.showAndWait();

        try {
            formula.setFormula(result.get(), m_block.ID, port.port().ID, key);
            port.setFormula(key, result.get());
        } catch (Exception e) {
//            System.out.println(result.get() + " " + m_block.ID + " " + port.port().ID + " " + key);
            //System.out.println("zase sa daco posralo");
            scheme().m_terminal.setText("Error, try again.");
            return;
        }
        m_block.addFormula(formula);
    }

    void registerFormula(GUIPort port, String key) {
        registerFormulaNoDisable(port, key);

        for (GUIPort tmp : m_outputPorts) {
            tmp.makeFormulaUnavailable();
        }
    }

    void getReady() {
        for (GUIPort port : m_inputPorts) {
            //System.out.println("port unready");

            m_unreadyPorts.add(port);
        }

        for (GUIPort port : m_outputPorts) {
            //System.out.println("port unready");

            m_unreadyPorts.add(port);
        }

        for (GUIPort port : m_inputPorts) {
            port.getReady();
        }

        for (GUIPort port : m_outputPorts) {
            port.getReady();
        }

        if (m_unreadyPorts.isEmpty())
            scheme().imReady(this);

    }

    void imReady(GUIPort port) {
        //System.out.println("port ready");

        m_unreadyPorts.remove(port);

        if (m_unreadyPorts.isEmpty())
            scheme().imReady(this);
    }

    void computingDone() {
        for (GUIPort port : m_outputPorts) {
            //System.out.println("updatujem value");
            port.updateValues();
        }

        for (GUIPort port : m_inputPorts) {
            //System.out.println("updatujem value");
            port.updateValues();
        }
    }

    JsonObject serializePorts()
    {
        JsonObjectBuilder tmp = Json.createObjectBuilder();

        int i = 0;

        for (GUIPort port: m_allPorts) {
            tmp.add("port" + i, port.serialize());
            ++i;
        }

        return tmp.build();
    }

    JsonObject serialize() {
        return Json.createObjectBuilder()
                .add("ID", block().ID.id())
                .add("Name", m_name.getText())
                .add("LayoutX", getLayoutX())
                .add("LayoutY", getLayoutY())
                .add("Ports", serializePorts())
                .build();
    }

    String getPortPosition(GUIPort port)
    {
        int i = 0;

        for (GUIPort tmp: m_allPorts) {
            if (tmp == port)
                break;

            ++i;

        }

        if (i > 9)
            scheme().m_terminal.setText("Error, try again.");

        return block().ID.id() + "-port-" + i;
    }

    void deserializePorts(JSONObject ports) throws Exception
    {
        int k = 0;
        for (GUIPort port: m_allPorts) {
            port.deserialize(ports.getJSONObject("port" + k));
            ++k;
        }
    }

    void pairPorts(Scheme scheme)
    {
        for (GUIPort port: m_inputPorts)
            port.m_port = scheme.findPort(port.m_id);

        for (GUIPort port: m_outputPorts)
            port.m_port = scheme.findPort(port.m_id);

    }

    public static GUIBlock deserialize(JSONObject block) throws Exception
    {
        GUIBlock tmp = new GUIBlock(null);

        ///todo
        tmp.m_name.setText(block.getString("Name"));
        tmp.setLayoutX(block.getDouble("LayoutX"));
        tmp.setLayoutY(block.getDouble("LayoutY"));
        tmp.m_id = new UniqueId("block", block.getInt("ID"));
        tmp.deserializePorts(block.getJSONObject("Ports"));
        return tmp;
    }

    GUIPort getPortByNumber(int number)
    {
        return m_allPorts.get(number);
    }

    void lightOn()
    {
        setStyle("-fx-background-color: yellow");

    }

    void lightOff()
    {
        setStyle("-fx-background-color: coral");

    }

    /// Members
    private double m_mouseX;
    private double m_mouseY;
    private GUIPort.Type m_toRegister;
    private Label m_name;
    private HashSet<GUIPort> m_availablePorts;
    private HashSet<GUIPort> m_outputPorts;
    private HashSet<GUIPort> m_inputPorts;
    private HashSet<GUIPort> m_unreadyPorts;
    private Vector<GUIPort> m_allPorts;
    private EventHandler m_onDrag;
    private EventHandler m_onPress;
    private ContextMenu m_menu;
     Block m_block;
    UniqueId m_id;
//    private GUIScheme m_scheme;
}
