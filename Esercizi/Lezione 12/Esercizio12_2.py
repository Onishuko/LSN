#!/usr/bin/env python
# coding: utf-8

# In[2]:


import tensorflow as tf
from tensorflow import keras
import numpy as np
import matplotlib.pyplot as plt

from keras.datasets import mnist
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten, Conv2D, MaxPooling2D
from keras.optimizers import SGD


# In[3]:


seed = 0
np.random.seed(seed)
tf.random.set_seed(seed)


# In[4]:


img_rows, img_cols = 28, 28
num_classes = 10

(X_train, Y_train), (X_test, Y_test) = mnist.load_data()

print("X_train shape:", X_train.shape)
print("Y_train shape:", Y_train.shape)
print("X_test shape:", X_test.shape)
print("Y_test shape:", Y_test.shape)


# In[5]:


#Normalization
X_train = X_train.astype("float32") / 255.0
X_test = X_test.astype("float32") / 255.0


# In[6]:


X_train = X_train.reshape(X_train.shape[0], img_rows, img_cols, 1)
X_test = X_test.reshape(X_test.shape[0], img_rows, img_cols, 1)

input_shape = (img_rows, img_cols, 1)

print("X_train shape:", X_train.shape)
print("X_test shape:", X_test.shape)


# In[7]:


Y_train = keras.utils.to_categorical(Y_train, num_classes)
Y_test = keras.utils.to_categorical(Y_test, num_classes)

print("Y_train shape:", Y_train.shape)
print("Y_test shape:", Y_test.shape)


# In[8]:


def create_CNN():
    model = Sequential()

    # First convolutional layer
    model.add(Conv2D(10, kernel_size=(5, 5), activation="relu", input_shape=input_shape))

    # First pooling layer
    model.add(MaxPooling2D(pool_size=(2, 2)))

    # Second convolutional layer
    model.add(Conv2D(16, kernel_size=(5, 5), activation="relu"))

    # Second pooling layer
    model.add(MaxPooling2D(pool_size=(2, 2)))

    # Convert feature maps into a vector
    model.add(Flatten())

    # Fully connected layer
    model.add(Dense(100, activation="relu"))

    # Dropout
    model.add(Dropout(0.5))

    # Output layer
    model.add(Dense(num_classes, activation="softmax"))

    # Compile
    model.compile(loss=keras.losses.categorical_crossentropy, optimizer=SGD(), metrics=["accuracy"])

    return model


# In[9]:


model_CNN = create_CNN()

model_CNN.summary()


# In[10]:


batch_size = 32
epochs = 40


# In[11]:


history_CNN = model_CNN.fit(X_train, Y_train, batch_size=batch_size, epochs=epochs, shuffle=True, verbose=1, validation_data=(X_test, Y_test))


# In[12]:


score_CNN = model_CNN.evaluate(X_test, Y_test, verbose=1)

print()
print("Test loss:", score_CNN[0])
print("Test accuracy:", score_CNN[1])


# In[13]:


plt.figure(figsize=(8, 5))

plt.plot(history_CNN.history["accuracy"], label="Training")
plt.plot(history_CNN.history["val_accuracy"], label="Validation")

plt.xlabel("Epoch")
plt.ylabel("Accuracy")
plt.title("CNN training and validation accuracy")
plt.legend()
plt.grid()
plt.show()


# In[14]:


plt.figure(figsize=(8, 5))

plt.plot(history_CNN.history["loss"], label="Training")
plt.plot(history_CNN.history["val_loss"], label="Validation")

plt.xlabel("Epoch")
plt.ylabel("Loss")
plt.title("CNN training and validation loss")
plt.legend()
plt.grid()
plt.show()


# In[21]:


predictions_CNN = model_CNN.predict(X_test)

predicted_labels = np.argmax(predictions_CNN, axis=1)
true_labels = np.argmax(Y_test, axis=1)

print("Predictions shape:", predictions_CNN.shape)


# In[23]:


indices = np.random.choice(len(X_test), 10, replace=False)

plt.figure(figsize=(15, 6))

for i, index in enumerate(indices):
    ax = plt.subplot(2, 5, i + 1)

    plt.imshow(X_test[index].reshape(28, 28), cmap="gray")

    plt.title(
        f"True: {true_labels[index]}\n"
        f"Predicted: {predicted_labels[index]}"
    )

    plt.axis("off")

plt.tight_layout()
plt.show()


# In[17]:


wrong_indices = np.where(predicted_labels != true_labels)[0]

print("Number of misclassified images:", len(wrong_indices))


# In[20]:


rng = np.random.default_rng(seed)
wrong_samples = rng.choice(wrong_indices, 10, replace=False)

plt.figure(figsize=(15, 6))

for i, index in enumerate(wrong_samples):
    ax = plt.subplot(2, 5, i + 1)

    plt.imshow(X_test[index].reshape(28, 28), cmap="gray")

    predicted = predicted_labels[index]
    confidence = predictions_CNN[index, predicted]

    plt.title(
        f"True: {true_labels[index]}\n"
        f"Pred: {predicted} ({confidence:.2%})"
    )

    plt.axis("off")

plt.tight_layout()
plt.show()


# ### Conclusion
# 
# The convolutional neural network achieved a test accuracy of 99.12%, significantly improving upon the DNN performance, while using only about 31,000 trainable parameters compared to more than 355,000 for the DNN. This improvement shows the advantage of exploiting the spatial structure of the images through convolutional and pooling layers. The analysis of randomly selected test images confirms the high classification accuracy.

# In[24]:


model_CNN.save("CNN_MNIST.keras")

