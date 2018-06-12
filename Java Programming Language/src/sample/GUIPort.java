package sample;

import javafx.scene.control.*;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import org.json.JSONObject;
import scheme.Port;
import unique.UniqueId;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonObjectBuilder;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Optional;

class GUIPort extends GridPane {

   private void preset()
   {
       m_type = Type.free;

       m_available = false;
       m_formulable = false;
       m_waitingForValues = false;
       m_waitingForFormula = false;
       m_values = new HashSet<>();
       m_unset = new HashSet<>();
       m_connections = new HashSet<>();

       m_tooltip = new Tooltip();
       Util.hackTooltipActivationTimer(m_tooltip, 0);
       Util.hackTooltipHideTimer(m_tooltip, 999999);
       m_tooltip.setText("Free port");
       Tooltip.install(this, m_tooltip);

       setStyle("-fx-background-color: blue");
       setVisible(false);
       setMinSize(20, 10);
       setMaxSize(20, 10);
       setPrefSize(20,10);

       setOnMouseClicked(event -> {
           if (m_available)
               parent().registerUsed(this);
       });

       m_port = null;
   }

    GUIPort(GUIBlock block)
    {

//        parent() = block;

        preset();

        setOnMousePressed(e -> {
            parent().disableDragging();
        });

        setOnMouseReleased(e -> {
            parent().enableDragging();
        });
    }

    private void createValue()
    {
        GUIPortValue tmp = new GUIPortValue("value_0" + m_values.size());
        TextInputDialog dialog = new TextInputDialog(tmp.getName());
        dialog.setTitle("Set value key");
        dialog.setHeaderText(null);
        dialog.setGraphic(null);
        dialog.setContentText("Key:");
        Optional<String> result = dialog.showAndWait();

        result.ifPresent(s -> tmp.setName(result.get()));

        m_values.add(tmp);
        reloadToolTip();

        registerRemove(tmp);

        port().updateData(tmp.getName(), Double.NaN);

    }

    private void registerRemove(GUIPortValue val)
    {

        MenuItem tmpMenuItem = new MenuItem(val.getName());
        tmpMenuItem.setOnAction(e -> {
            m_values.remove(val);
            unregisterRemove(tmpMenuItem);
            reloadToolTip();
            port().dataSet.remove(val.getName());
        });

        m_removeMenu.getItems().add(tmpMenuItem);
    }

    private void unregisterRemove(MenuItem item)
    {
        m_removeMenu.getItems().remove(item);
        m_removeMenu.hide();
    }

    private void reloadToolTip()
    {
        String text = m_type.name() + " port";
        text = text.toUpperCase();

        for (GUIPortValue tmp : m_values){
            text += "\n" + tmp.stringify();
        }
        m_tooltip.setText(text);
    }

    void appendAvailable()
    {
        setStyle("-fx-background-color: green");
        setOpacity(0.1);
        setVisible(true);
        m_available = true;
    }

    void reset()
    {
        disappear();
        preset();
    }

    void disappear()
    {
        m_available = false;
        setVisible(false);

    }

    private void allow()
    {


        // add info allowing to identify this node as drag source/target
        setUserData(Boolean.TRUE);

        MenuItem addValue = new MenuItem("Add value");
        addValue.setOnAction(t -> {
            createValue();
        });

        MenuItem delete = new MenuItem("Delete");
        delete.setOnAction(t -> {
            parent().registerUnused(this);
        });

        m_removeMenu = new Menu("Remove", null);

        final ContextMenu cm = new ContextMenu();
        cm.addEventFilter(MouseEvent.MOUSE_RELEASED, event -> {
            if (event.getButton() == MouseButton.SECONDARY) {
                //System.out.println("consuming right release button in cm filter");
                event.consume();
            }
        });

        cm.getItems().addAll(addValue,
                             m_removeMenu,
                             new SeparatorMenuItem(),
                             delete);

        setOnMouseClicked(e -> {
            if (e.getButton() == MouseButton.SECONDARY) {
                //System.out.println("Port right click");

                cm.show(this, e.getScreenX(), e.getScreenY());
            }
            else {
                //System.out.println("Port no right click click");

                if (cm.isShowing())
                    cm.hide();

                if (m_formulable){
                    appendMenuFormula();
                    m_formulaMenu.show(this, e.getScreenX(), e.getScreenY());
                }

                if (m_waitingForValues) {
                    appendMenuValues();
                    m_valuesMenu.show(this, e.getScreenX(), e.getScreenY());
                }

                if (m_waitingForFormula) {
                    appendMenuuUnsetFormula();
                    m_formulaMenu.show(this, e.getScreenX(), e.getScreenY());
                }
            }
            e.consume();
        });

        //System.out.println("zecte ma");
        setOnDragDetected(parent().scheme().m_dragStart);
        setOnMouseDragReleased(parent().scheme().m_dragRelease);
        setOnMouseDragEntered(parent().scheme().m_dragEnter);
//        setOnDragDetected(e -> System.out.println("NASRAT"));
        setUserData(Boolean.TRUE);
    }

    void setAsIn()
    {
        m_type = Type.input;
        m_available = false;
        reloadToolTip();
        setStyle("-fx-background-color: darkred");

        setOpacity(1);
        setVisible(true);
        allow();

        m_port = new Port();
        m_port.type = 0;
    }

    void setAsOut()
    {
        m_type = Type.output;
        m_available = false;
        reloadToolTip();
        setStyle("-fx-background-color: darkblue");
        setOpacity(1);
        setVisible(true);
        allow();

        m_port = new Port();
        m_port.type = 1;
    }

    Type type()
    {
        return m_type;
    }

    enum Type {
        input,
        output,
        free
    }

    GUIBlock parent()
    {
        return (GUIBlock)getParent();
    }

    Port port()
    {
        return m_port;
    }

    void makeFormulaAvailable()
    {
        setStyle("-fx-background-color: yellow");
        m_formulable = true;

    }

    void makeFormulaUnavailable()
    {
        setStyle("-fx-background-color: darkblue");
        m_formulable = false;
    }

    void appendMenuFormula()
    {
        m_formulaMenu = new ContextMenu();
        m_formulaMenu.addEventFilter(MouseEvent.MOUSE_RELEASED, event -> {
            if (event.getButton() == MouseButton.SECONDARY) {
                //System.out.println("consuming right release button in cm filter");
                event.consume();
            }
        });

        for (GUIPortValue val: m_values){
            MenuItem tmp = new MenuItem(val.getName());
            tmp.setOnAction(t -> {
                parent().registerFormula(this, val.getName());
            });
            m_formulaMenu.getItems().add(tmp);
        }
    }

    void appendMenuuUnsetFormula()
    {
        m_formulaMenu = new ContextMenu();
        m_formulaMenu.addEventFilter(MouseEvent.MOUSE_RELEASED, event -> {
            if (event.getButton() == MouseButton.SECONDARY) {
                //System.out.println("consuming right release button in cm filter");
                event.consume();
            }
        });

        for (GUIPortValue val: m_unset){
            MenuItem tmp = new MenuItem(val.getName());
            tmp.setOnAction(t -> {
                parent().registerFormulaNoDisable(this, val.getName());
                m_unset.remove(val);
                m_formulaMenu.getItems().remove(tmp);
                if (m_formulaMenu.getItems().isEmpty()) {
                    m_waitingForFormula = false;
                    setStyle("-fx-background-color: darkblue");
                    parent().imReady(this);
                }

            });
            m_formulaMenu.getItems().add(tmp);
        }
    }

    void appendMenuValues()
    {
        m_valuesMenu = new ContextMenu();
        m_valuesMenu.addEventFilter(MouseEvent.MOUSE_RELEASED, event -> {
            if (event.getButton() == MouseButton.SECONDARY) {
                //System.out.println("consuming right release button in cm filter");
                event.consume();
            }
        });

        for (GUIPortValue val: m_unset){
            MenuItem tmp = new MenuItem(val.getName());
            //marhy
            tmp.setOnAction(t -> {

                val.setValue(inputValue());
                port().updateData(val.getName(), val.getValue());
                m_unset.remove(val);
                reloadToolTip();

                if (m_unset.isEmpty()) {
                    m_waitingForValues = false;
                    setStyle("-fx-background-color: darkred");
                    parent().imReady(this);
                }
            });
            m_valuesMenu.getItems().add(tmp);
        }
    }

    double inputValue()
    {
        TextInputDialog dialog = new TextInputDialog("0");
        dialog.setTitle("Set value");
        dialog.setHeaderText(null);
        dialog.setGraphic(null);
        dialog.setContentText("value:");
        Optional<String> result = dialog.showAndWait();

        return Double.valueOf(result.get());
    }

    void getReady()
    {

        if (type() == Type.input) {

            for (GUIPortValue value: m_values) {
                String key = value.getName();
                Boolean found = false;

                for (GUIConnection connection: m_connections) {
                    if (connection.gotOutputValue(key)) {
                        found = true;
                    }
                }

                if (!found)
                    m_unset.add(value);
            }
            ///todo fill

            if (m_unset.isEmpty()) {
                parent().imReady(this);
                return;
            }

            m_waitingForValues = true;
            setStyle("-fx-background-color: lightgreen");
        }
        else {
            for (GUIPortValue value: m_values) {
                if (value.getFormula().isEmpty()) {
                    m_unset.add(value);
                }
            }

            if (m_unset.isEmpty()) {
                parent().imReady(this);
                return;
            }

            m_waitingForFormula = true;
            setStyle("-fx-background-color: yellow");
        }


    }

    Boolean hasValue(String key)
    {
        for (GUIPortValue value: m_values) {
            if (value.getName().equals(key))
                return true;
        }
        return false;
    }

    void addConnection(GUIConnection connection)
    {
        m_connections.add(connection);
    }

    void removeConnection(GUIConnection connection)
    {
        m_connections.remove(connection);
    }

    void updateValues()
    {
        for (GUIPortValue value: m_values) {
            //System.out.println("nastaqvujem hodnotu " + port().getValue(value.getName()));
            value.setValue(port().getValue(value.getName()));

            if (type() == Type.input)
            {
                if (port().pairPort != null) {
                    if (parent().scheme().m_scheme.findPort(port().pairPort).hasItem(value.getName()))
                        value.setValue(parent().scheme().m_scheme.findPort(port().pairPort).getValue(value.getName()));
                }
            }
        }
        reloadToolTip();
    }

    void setFormula(String key, String formula)
    {
        for (GUIPortValue value : m_values) {
            if (value.getName().equals(key))
                value.setFormula(formula);
        }
    }

    JsonObject serialize()
    {
        JsonObjectBuilder builder = Json.createObjectBuilder();

        JsonObjectBuilder valueBuilder = Json.createObjectBuilder();

        builder.add("type", type().name());
        if (type() != Type.free)
            builder.add("ID", port().ID.id());

        for (GUIPortValue value: m_values) {
            valueBuilder.add(value.getName(), value.getFormula());
        }
        builder.add("values", valueBuilder.build());

        return builder.build();
    }


    void deserialize(JSONObject port) throws Exception
    {
        m_type = Type.valueOf(port.getString("type"));
        if (type() != Type.free)
            m_id = new UniqueId("port", port.getInt("ID"));

        JSONObject values = port.getJSONObject("values");


        for (Iterator iter = values.keys(); iter.hasNext(); ) {
            Object keyO = iter.next();
            String key = keyO.toString();

            GUIPortValue tmp = new GUIPortValue(key);
            tmp.setFormula(values.getString(key));
            m_values.add(tmp);
        }


        parent().registerUsed(this, type());
    }

    // MEMBERS
    private HashSet<GUIConnection> m_connections;
    private Tooltip m_tooltip;
//    private GUIBlock parent();
    private Boolean m_available;
    private HashSet<GUIPortValue> m_values;
    private HashSet<GUIPortValue> m_unset;
    private Type m_type;
    private Menu m_removeMenu;
    Port m_port;
    private Boolean m_formulable;
    private Boolean m_waitingForValues;
    private Boolean m_waitingForFormula;
    private ContextMenu m_formulaMenu;
    private ContextMenu m_valuesMenu;
     UniqueId m_id;
}
