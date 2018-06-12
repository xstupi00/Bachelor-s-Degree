package sample;


import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.stage.Window;

public class Main extends Application {

    public static Window stage;

    @Override
    public void start(Stage primaryStage) throws Exception{
        FXMLLoader loader = new FXMLLoader(getClass().getResource("stage.fxml"));
        GUIController controller = new GUIController();
        loader.setController(controller);
        Parent root = loader.load();
        controller.preset();

        primaryStage.setTitle("Block Scheme Editor");
        primaryStage.setScene(new Scene(root));
        primaryStage.setMinHeight(400);
        primaryStage.setMinWidth(400);
        primaryStage.show();

        stage = primaryStage;
    }

    public static void main(String[] args) {launch(args);}

}
