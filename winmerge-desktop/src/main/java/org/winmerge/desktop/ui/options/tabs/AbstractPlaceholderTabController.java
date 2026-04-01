package org.winmerge.desktop.ui.options.tabs;

import javafx.beans.binding.Bindings;
import javafx.beans.property.IntegerProperty;
import javafx.fxml.FXML;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Spinner;
import javafx.scene.control.SpinnerValueFactory;
import javafx.scene.control.TextInputControl;
import org.winmerge.desktop.ui.options.AppSettings;

public abstract class AbstractPlaceholderTabController implements OptionsTabController {
    @FXML
    private Parent contentRoot;

    @Override
    public void bind(AppSettings settings) {
        if (settings == null) {
            throw new IllegalArgumentException("settings must not be null");
        }
        if (contentRoot == null) {
            throw new IllegalStateException("Missing @FXML injection for contentRoot");
        }
        bindNode(contentRoot, settings);
    }

    private void bindNode(Node node, AppSettings settings) {
        if (node instanceof CheckBox checkBox) {
            bindBoolean(checkBox, settings);
        } else if (node instanceof Spinner<?> rawSpinner) {
            bindInteger(rawSpinner, settings);
        } else if (node instanceof TextInputControl textInputControl) {
            bindString(textInputControl, settings);
        }
        if (node instanceof Parent parent) {
            for (Node child : parent.getChildrenUnmodifiable()) {
                bindNode(child, settings);
            }
        }
    }

    private static void bindBoolean(CheckBox checkBox, AppSettings settings) {
        String spec = spec(checkBox);
        if (!spec.startsWith("bool|")) {
            return;
        }
        String[] tokens = spec.split("\\|", 3);
        if (tokens.length < 3) {
            throw new IllegalStateException("Invalid bool binding spec: " + spec);
        }
        String key = tokens[1];
        boolean defaultValue = Boolean.parseBoolean(tokens[2]);
        Bindings.bindBidirectional(
            checkBox.selectedProperty(),
            settings.booleanProperty(key, defaultValue)
        );
    }

    private static void bindInteger(Spinner<?> rawSpinner, AppSettings settings) {
        String spec = spec(rawSpinner);
        if (!spec.startsWith("int|")) {
            return;
        }
        String[] tokens = spec.split("\\|", 6);
        if (tokens.length < 5) {
            throw new IllegalStateException("Invalid int binding spec: " + spec);
        }

        String key = tokens[1];
        int defaultValue = Integer.parseInt(tokens[2]);
        int min = Integer.parseInt(tokens[3]);
        int max = Integer.parseInt(tokens[4]);

        @SuppressWarnings("unchecked")
        Spinner<Integer> spinner = (Spinner<Integer>) rawSpinner;
        IntegerProperty property = settings.integerProperty(key, defaultValue);
        SpinnerValueFactory.IntegerSpinnerValueFactory valueFactory =
            new SpinnerValueFactory.IntegerSpinnerValueFactory(min, max, clamp(property.get(), min, max));
        spinner.setValueFactory(valueFactory);
        spinner.setEditable(true);
        property.addListener((obs, oldValue, newValue) -> {
            int clamped = clamp(newValue.intValue(), min, max);
            if (spinner.getValue() == null || spinner.getValue() != clamped) {
                spinner.getValueFactory().setValue(clamped);
            }
        });
        spinner.valueProperty().addListener((obs, oldValue, newValue) -> {
            if (newValue != null) {
                property.set(clamp(newValue, min, max));
            }
        });
    }

    private static void bindString(TextInputControl control, AppSettings settings) {
        String spec = spec(control);
        if (!spec.startsWith("string|")) {
            return;
        }
        String[] tokens = spec.split("\\|", 3);
        if (tokens.length < 3) {
            throw new IllegalStateException("Invalid string binding spec: " + spec);
        }
        String key = tokens[1];
        String defaultValue = tokens[2];
        Bindings.bindBidirectional(
            control.textProperty(),
            settings.stringProperty(key, defaultValue)
        );
    }

    private static String spec(Node node) {
        Object userData = node.getUserData();
        return userData == null ? "" : userData.toString().trim();
    }

    private static int clamp(int value, int min, int max) {
        return Math.max(min, Math.min(max, value));
    }
}
