package org.winmerge.desktop;

import java.io.IOException;

import javafx.animation.PauseTransition;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.util.Duration;
import org.winmerge.desktop.ui.MainController;

public class WinMergeApp extends Application {
    private static final String SMOKE_TEST_ARG = "--smoke-test";

    @Override
    public void start(Stage primaryStage) throws IOException {
        FXMLLoader loader = new FXMLLoader(WinMergeApp.class.getResource("/org/winmerge/desktop/ui/MainWindow.fxml"));
        Parent root = loader.load();

        MainController mainController = loader.getController();
        mainController.setPrimaryStage(primaryStage);

        Scene scene = new Scene(root, 1200, 760);
        scene.getStylesheets().add(
            WinMergeApp.class.getResource("/org/winmerge/desktop/style/winmerge.css").toExternalForm()
        );

        primaryStage.setTitle("WinMerge");
        primaryStage.setScene(scene);
        primaryStage.setOnCloseRequest(mainController::handleWindowCloseRequest);
        primaryStage.show();

        if (getParameters().getRaw().contains(SMOKE_TEST_ARG)) {
            PauseTransition pause = new PauseTransition(Duration.seconds(1));
            pause.setOnFinished(event -> Platform.exit());
            pause.play();
        }
    }

    public static void main(String[] args) {
        launch(args);
    }
}
