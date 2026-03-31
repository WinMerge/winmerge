package org.winmerge.desktop;

import javafx.animation.PauseTransition;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;
import javafx.util.Duration;

public class WinMergeDesktopApp extends Application {
    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("WinMerge JavaFX Migration Scaffold");
        primaryStage.setScene(new Scene(new StackPane(new Label("WinMerge Desktop Scaffold")), 800, 600));
        primaryStage.show();

        if (getParameters().getRaw().contains("--smoke-test")) {
            PauseTransition pause = new PauseTransition(Duration.seconds(1));
            pause.setOnFinished(event -> Platform.exit());
            pause.play();
        }
    }

    public static void main(String[] args) {
        launch(args);
    }
}
