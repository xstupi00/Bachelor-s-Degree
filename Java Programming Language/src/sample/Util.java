package sample;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.scene.control.Tooltip;
import javafx.util.Duration;

import java.lang.reflect.Field;

/**
 * Created by lemmi on 26.09.17.
 */
class Util {

    private static double mouseX;
    private static double mouseY;

//    public static void installWindowDragListener(GUIBlock node) {
//        node.setOnMousePressed(evt -> {
//            mouseX = evt.getSceneX();
//            mouseY = evt.getSceneY();
//        });
//
//        node.setOnMouseDragged(evt -> {
//            double deltaX = evt.getSceneX() - mouseX;
//            double deltaY = evt.getSceneY() - mouseY;
//
//            double toSetX = node.getLayoutX() + deltaX;
//            double toSetY = node.getLayoutY() + deltaY;
//
//            if (toSetX > node.parent().getWidth() - node.getWidth())
//                toSetX = node.parent().getWidth() - node.getWidth();
//
//            if (toSetY > node.parent().getHeight() - node.getHeight())
//                toSetY = node.parent().getHeight() - node.getHeight();
//
//            if (toSetX < 0)
//                toSetX = 0;
//
//            if (toSetY < 0)
//                toSetY = 0;
//
//            node.setLayoutX(toSetX);
//            node.setLayoutY(toSetY);
//
//            mouseX = evt.getSceneX();
//            mouseY = evt.getSceneY();
//        });
//
//    }


    public static void hackTooltipActivationTimer( Tooltip tooltip, int delay ) {
        hackTooltipTiming( tooltip, delay, "activationTimer" );
    }

    public static void hackTooltipHideTimer( Tooltip tooltip, int delay ) {
        hackTooltipTiming( tooltip, delay, "hideTimer" );
    }

    private static void hackTooltipTiming( Tooltip tooltip, int delay, String property ) {
        try {
            Field fieldBehavior = tooltip.getClass().getDeclaredField( "BEHAVIOR" );
            fieldBehavior.setAccessible( true );
            Object objBehavior = fieldBehavior.get( tooltip );

            Field fieldTimer = objBehavior.getClass().getDeclaredField( property );
            fieldTimer.setAccessible( true );
            Timeline objTimer = (Timeline) fieldTimer.get( objBehavior );

            objTimer.getKeyFrames().clear();
            objTimer.getKeyFrames().add( new KeyFrame( new Duration( delay ) ) );
        }
        catch ( Exception e ) {
            e.printStackTrace();
        }
    }

}
