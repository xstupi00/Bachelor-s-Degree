package sample;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.MenuItem;
import javafx.scene.input.MouseDragEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;



public class GUIController {
    @FXML private Pane schemePane;
    @FXML private Label msg;
    @FXML private MenuItem saveItem;
    @FXML private MenuItem loadItem;
    @FXML private MenuItem newItem;
    @FXML private Button step;

    public static GUIScheme m_scheme;

    @FXML
    protected void buttonAdd(ActionEvent event)
    {
        m_scheme.addBlock(new GUIBlock(m_scheme));
    }

//    @FXML
//    protected void buttonStep(ActionEvent event)
//    {
//        m_scheme.calcStep();
//    }

    public void GUIController() {

    }


    @FXML
    protected void buttonCalc(ActionEvent event)
    {
        m_scheme.calculate();
    }

    public void  preset()
    {
        m_scheme = new GUIScheme(schemePane, msg, this);
        saveItem.setOnAction((ActionEvent e) -> {
            m_scheme.saveScheme();
        });

        loadItem.setOnAction((ActionEvent e) -> {
            m_scheme.loadScheme();
        });
        newItem.setOnAction((ActionEvent e) -> {
            m_scheme.resetAll();
        });
        stepGetReady();
    }

    void stepDone()
    {
        step.setText("Done");
        step.setOnAction(e -> {
            m_scheme.stepsDone();
            stepGetReady();
        });
    }

    void stepGetReady()
    {
        step.setText("Prep");
        step.setOnAction(e -> {
            m_scheme.stepsGetReady();
        });
    }

    void stepStep()
    {
        step.setText("Step");
        step.setOnAction(e -> {
            m_scheme.calcStep();
        });
    }
}

