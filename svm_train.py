import pandas as pd
import matplotlib.pyplot as plt
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix, ConfusionMatrixDisplay
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
import joblib

df = pd.read_csv("feature_dataset.csv")

X = df.drop("label", axis=1)
y = df["label"]

X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42, stratify=y
)

scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

print("=== COPY KE KODE ESP32 ===")
print("float scaler_mean[7] = {", ", ".join(map(str, scaler.mean_)), "};")
print("float scaler_scale[7] = {", ", ".join(map(str, scaler.scale_)), "};")
print("==========================\n")

model = SVC(kernel='rbf', gamma=0.1, random_state=42)
model.fit(X_train, y_train)

pred = model.predict(X_test)

acc = accuracy_score(y_test, pred)
print(f"Accuracy: {acc * 100:.2f}%\n")

print("=== CLASSIFICATION REPORT ===")
print(classification_report(y_test, pred, target_names=["NORMAL", "FALL"]))
print("=============================\n")

print("=== CONFUSION MATRIX ===")
cm = confusion_matrix(y_test, pred)
print(cm)
print("========================\n")

joblib.dump(model, "svm_model.pkl")
print("Model berhasil disimpan ke 'svm_model.pkl'")

disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=["NORMAL", "FALL"])
disp.plot(cmap=plt.cm.Blues)
plt.title("Confusion Matrix - Fall Detection")
plt.show()