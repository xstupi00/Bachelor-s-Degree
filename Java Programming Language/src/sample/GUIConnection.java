package sample;

import javafx.geometry.Bounds;
import javafx.scene.Node;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tooltip;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;
import javafx.scene.shape.Line;
import org.json.JSONObject;
import scheme.Connection;
import scheme.Scheme;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonObjectBuilder;

public class GUIConnection extends Line {

    Node m_begin;
    Node m_end;

    GUIPort m_input;
    GUIPort m_output;
    ContextMenu m_menu;
    Connection m_connection;

    Connection connection() {
        return m_connection;
    }

    public GUIConnection() {
        super();
        super.setStrokeWidth(3);
    }

    public void setStartPort(Node source) {
        m_begin = source;
        Bounds bounds = source.getBoundsInParent();
        Bounds bounds_parent = source.getParent().getBoundsInParent();
        super.setStartX(bounds_parent.getMinX() + (bounds.getMinX() + bounds.getMaxX()) / 2);
        super.setStartY(bounds_parent.getMinY() + (bounds.getMinY() + bounds.getMaxY()) / 2);

        source.getParent().boundsInParentProperty().addListener(e -> {
            this.recalcBegin();
        });
    }

    void recalcBegin() {
        Bounds bounds = m_begin.getBoundsInParent();
        Bounds bounds_parent = m_begin.getParent().getBoundsInParent();
        super.setStartX(bounds_parent.getMinX() + (bounds.getMinX() + bounds.getMaxX()) / 2);
        super.setStartY(bounds_parent.getMinY() + (bounds.getMinY() + bounds.getMaxY()) / 2);
    }

    public void centerIfShould(Node destination) {
//        if (((GUIPort)destination).type() == ((GUIPort)m_begin).type())
//            return;

        Bounds bounds = destination.getBoundsInParent();
        Bounds bounds_parent = destination.getParent().getBoundsInParent();
        super.setEndX(bounds_parent.getMinX() + (bounds.getMinX() + bounds.getMaxX()) / 2);
        super.setEndY(bounds_parent.getMinY() + (bounds.getMinY() + bounds.getMaxY()) / 2);
    }

    public void setEndPort(Node destination, Boolean notnew) {
        m_end = destination;
        Bounds bounds = m_end.getBoundsInParent();
        Bounds bounds_parent = m_end.getParent().getBoundsInParent();
        super.setEndX(bounds_parent.getMinX() + (bounds.getMinX() + bounds.getMaxX()) / 2);
        super.setEndY(bounds_parent.getMinY() + (bounds.getMinY() + bounds.getMaxY()) / 2);

        controll(notnew);

        destination.getParent().boundsInParentProperty().addListener(e -> {
            this.recalcEnd();
        });
    }

    public void setEndPort(Node destination)
    {
        setEndPort(destination, false);
    }

    void recalcEnd() {
        Bounds bounds = m_end.getBoundsInParent();
        Bounds bounds_parent = m_end.getParent().getBoundsInParent();
        super.setEndX(bounds_parent.getMinX() + (bounds.getMinX() + bounds.getMaxX()) / 2);
        super.setEndY(bounds_parent.getMinY() + (bounds.getMinY() + bounds.getMaxY()) / 2);
    }

    void controll(Boolean notNew) {
        toFront();

        //System.out.println("KONTROLUJEM TO LUDIA MOJI VIETE O TOM?");
        m_input = (GUIPort) m_begin;
        m_output = (GUIPort) m_end;

        if (m_input.type() == m_output.type() || m_input.parent() == m_output.parent())
            ((Pane) getParent()).getChildren().remove(this);

        if (m_input.type() == GUIPort.Type.output) {
            GUIPort tmp = m_input;
            m_input = m_output;
            m_output = tmp;
        }

        setMenu();

        if (!notNew)
            m_connection = new Connection();
        try {
            //System.out.println(m_input.port());
            //System.out.println(m_output.port());
            Scheme tmp = GUIController.m_scheme.m_scheme;

            if (!notNew)
                m_connection.createConnection(m_input.port(), m_output.port());

        } catch (Exception e) {
            try {

                ((Pane) getParent()).getChildren().remove(this);
            } catch (NullPointerException i) {
                m_input.parent().scheme().m_terminal.setText("Error, try again.");
            }
            m_input.parent().scheme().m_terminal.setText("Ports don't have any common value keys!");
            return;
        }
        if (!notNew)
            m_input.parent().scheme().addConnection(this);

        m_input.addConnection(this);
        if (!notNew)
            createToolTip();

        setOnMouseEntered(e -> {
            refreshToolTip();
        });
    }

    void setMenu() {

        // MENU ITEM - Delete
        MenuItem delete = new MenuItem("Delete");
        delete.setOnAction(t -> {
            m_input.parent().scheme().removeConnection(this);
            m_input.removeConnection(this);
        });

        m_menu = new ContextMenu();

        m_menu.addEventFilter(MouseEvent.MOUSE_RELEASED, event -> {
            if (event.getButton() == MouseButton.SECONDARY) {
                //System.out.println("Right-click on block menu. Consuming event.");
                event.consume();
            }
        });

        m_menu.getItems().add(delete);

        setOnMouseClicked(e -> {
            if (e.getButton() == MouseButton.SECONDARY) {

                m_menu.show(this, e.getScreenX(), e.getScreenY());
            } else {

                if (m_menu.isShowing())
                    m_menu.hide();
            }
        });
    }

    Boolean gotOutputValue(String key) {
        return m_output.hasValue(key);
    }


    void createToolTip() {
        m_tooltip = new Tooltip();

        Util.hackTooltipActivationTimer(m_tooltip, 0);
        Util.hackTooltipHideTimer(m_tooltip, 999999);
        m_tooltip.setText(m_connection.viewData(GUIController.m_scheme.m_scheme));
        Tooltip.install(this, m_tooltip);
    }

    void refreshToolTip() {
        if (m_tooltip == null)
            createToolTip();

        //System.out.println(m_tooltip);
        //System.out.println(GUIController.m_scheme);
        //System.out.println(GUIController.m_scheme.m_scheme);
        m_tooltip.setText(m_connection.viewData(GUIController.m_scheme.m_scheme));
    }

    JsonObject serialize() {
        JsonObjectBuilder builder = Json.createObjectBuilder();
        builder.add("input", m_input.parent().getPortPosition(m_input));
        builder.add("output", m_output.parent().getPortPosition(m_output));

        return builder.build();
    }

    static GUIConnection deserialize(JSONObject connection) throws Exception
    {

        GUIConnection tmp = new GUIConnection();
        String[] arr = connection.getString("input").split("-port-");
        //System.out.println(arr);
        tmp.setStartPort(GUIController.m_scheme.getBlockById(Integer.valueOf(arr[0])).getPortByNumber(Integer.valueOf(arr[1])));


        arr = connection.getString("output").split("-port-");
        tmp.setEndPort(GUIController.m_scheme.getBlockById(Integer.valueOf(arr[0])).getPortByNumber(Integer.valueOf(arr[1])), true);

        GUIController.m_scheme.m_pane.getChildren().add(tmp);
        GUIController.m_scheme.m_connections.add(tmp);
        return tmp;
    }

    Tooltip m_tooltip;
}
