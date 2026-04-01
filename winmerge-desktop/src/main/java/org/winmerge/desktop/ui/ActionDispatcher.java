package org.winmerge.desktop.ui;

import java.util.EnumMap;
import java.util.Map;
import java.util.Objects;

import javafx.application.Platform;

public class ActionDispatcher {
    private final Map<ActionId, Runnable> actionHandlers = new EnumMap<>(ActionId.class);

    public void register(ActionId actionId, Runnable handler) {
        actionHandlers.put(Objects.requireNonNull(actionId, "actionId"), Objects.requireNonNull(handler, "handler"));
    }

    public void dispatch(ActionId actionId) {
        Runnable handler = actionHandlers.get(Objects.requireNonNull(actionId, "actionId"));
        if (handler == null) {
            throw new IllegalStateException("No action registered for " + actionId);
        }
        if (Platform.isFxApplicationThread()) {
            handler.run();
        } else {
            Platform.runLater(handler);
        }
    }
}
