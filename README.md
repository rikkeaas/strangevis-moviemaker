<div align="center">
	<img src="assets/app_preview_5_updated_colors_shadow_large_spread.png">
	<h1>Strangevis Movie Maker</h1>
	<p>
		<b>A visualization tool for exploring and analyzing volumetric data</b>
	</p>
	<br>
	<br>
</div>

## Project Tasks

- Design and implement a visualization tool for the exploration and analysis of volumetric data
- Learn how to translate task requirements into a working piece of software
- Think about the objectives before thinking about the realization
- Iterate and revise your design in a collaborative manner
- Have fun while doing it - inspire your colleagues with our ideas and be inspired

## Features

### 3D Volume Rendering

The main task of our program was to have the ability to load and render a volumetric model.

TODO

### Animation

The `Movie Maker` part of our program hints about presenting the data in a way which might look like a movie. To achieve this, we needed to implement a few key features; `State Saving`, `State Loading` and `Interpolation` for smooth transitions between the states.

#### States

A state can be saved in two different ways; either using the <kbd>K</kbd> key, or using the large <kbd>+</kbd> button in the `Keyframe Handler` in the lower left corner of the screen. A state is then saved to a local folder, and contains information about the `projection`, `rotation`, `scaling`, and `translation` matrices used with the current volume in view, in addition to the `background color`, `transfer function` data and each `layer` used. A low-resolution snapshot is also saved with each state, which together creates the Keyframes the Keyframe Handler is presenting.

A state is loaded on demand, and can be triggered by clicking on a saved Keyframe. This initializes a function which updates the matrices to the corresponding ones included in the state file, and the same goes for the other contents saved in the state. The results of a state read is that the volume looks exactly like the corresponding low-res snapshot which was clicked to trigger the state read.

#### Interpolation

Updating the states without interpolating to the new one would not look movie like at all, so we wanted to implement some kind of interpolation to achieve this feeling of smooth animations between two different states. When a state read is initialized, every part of the loaded state will be loaded while being interpolated with a length or step size controlled by the user.

While the interpolation between one state and another looks great using linear interpolation, this would not look equally great when interpolating between several states. To make this kind of interpolation look much better, we also implemented a Catmull-Rom interpolation, making the transitions between many states look much smoother.

The Catmull-Rom interpolation enabled us to interpolate smoothly through all saved states. To start playing through all states, press <kbd>A</kbd>. The type of interpolation can be changed via the menubar, in addition to the option for setting a user specified duration.

### Histogram

The histogram is a central part of the application. It is the "man in the middle" when selecting layers, and reflects the density of the volume being rendered. It is generated on volume load, and refreshes if a new volume has been loaded.

We have implemented three different modes for data display in the histogram. The standard mode is the `logarithmic`, where the data is displayed using a logarithmic y-axis.

Another mode is the `linear` mode, which presents the data as it is in the dataset. While this represents the data in the most accurate way, some datasets with densities occuring a lot will make the less occuring densities hard to interpret. Therefore, we have added a final mode for data representation in the histogram, which we have called `linear with clamp`. Here, the user can define the clamp-value, which makes the densities with low occurences much more visible, and moving the focus away from the high-occurence densities.

Doing a <kbd>Click and Drag</kbd> action on the histogram will select the corresponding data in the volume, and a `QColorDialog` will pop-up and give the user the ability to set the color and opacity for this data. This action creates a new `Layer`, which is explained in detail below.

### Layers

When the histogram has been used to create a new layer, this will be given the default name `Layer name...`. The layer label is a `QLineEdit`, and renaming the layers is done by typing into this text field. The layer widget includes a small color sample representing the selected color for the corresponding layer, and pressing this will open the `QColorDialog` again, enabling color and opacity changing.

Each layer stores the bounds of the data selection, color, opacity and the label text of the layer.

To delete a layer, simply <kbd>Double Click</kbd> on the outermost parts of the layer widgets.

### Cutting Tool

TODO

### Transfer Function

TODO

### Other

#### Volume Screenshot

Under the <kbd>File</kbd> item in the menubar there is an option for creating high resolution screenshots of the `OpenGLWidget`, which is the widget where the volume is being rendered. The screenshots are saved inside a project folder named <kbd>screenshots</kbd>.

#### Background Coloring

Pressing <kbd>B</kbd> or using the menubar item <kbd>Edit</kbd>-><kbd>Choose Background Color</kbd> will open the `QColorDialog`, which provides the ability for the user to set a custom background color for the `OpenGLWidget`.

#### Multithreaded Volume Loading

To avoid making the application not responding while a new volume is being loaded, we have implemented multithreading for loading on another thread. When the volume loading has completed without errors, it sends a `signal` to redraw the histogram and update the keyframes being show in the `Keyframe Handler`. This makes the user experience more seamless, without interrupting the workflow when loading large datasets.

## Project Planning

Before we began implementing the program, we followed the [Five Design Sheet Methodology](http://fds.design/). The methodology starts with a brain storming process where all our ideas were written down on the first design sheet. Extracting the most exciting features from the first sheet, we created three completely different designs, where each had its own advantages over the others. The final design sheet was a combination of these, where the most important features from every design was selected to be implemented in our project. Our final design looked like this:

![Final Design Sheet](assets/final_design.png)

# Installation

TODO

# Usage

TODO

## Keyboard Shortcuts and Mouse Actions

- <kbd>A</kbd>: Interpolate through all saved states
- <kbd>B</kbd>: Choose background color for `OpenGLWidget`
- <kbd>C</kbd>: Clear all saved states
- <kbd>K</kbd>: Save state
- <kbd>Shift</kbd><kbd>Click on Keyframe</kbd>: Delete selected keyframe
- <kbd>Shift</kbd><kbd>Mouse Drag</kbd>: Rotate volume
- <kbd>Mouse drag</kbd>: Translate volume
- <kbd>Mouse scroll</kbd>: Scale volume

## Menubar Actions

Items in the <kbd>File</kbd> menu:

- <kbd>Open</kbd>: Open a `QFileDialog` for selecting a volume to load
- <kbd>Screenshot</kbd>: Save a screenshot of the `OpenGLWidget`

Items in the <kbd>Edit</kbd> menu:

- <kbd>Choose Background Color</kbd>: Choose background color for `OpenGLWidget
- <kbd>Set ray sampling distance multiplier</kbd>: TODO
- <kbd>Toggle light/volume transformation</kbd>: Toggle mode for transforming the volume or the light

Items in the <kbd>Animation</kbd> menu:

- <kbd>Set type of interpolation</kbd>: Set the type of interpolation used in the animation
- <kbd>Adjust Animation Duration</kbd>: Set the duration of a single interpolation between two states
- <kbd>Clear all states</kbd>: Clear all saved states

Items in the <kbd>Cut</kbd> menu:

- <kbd>Cut type</kbd>: Choose type of cut
- <kbd>Set cut radius (spherical cut)</kbd>: If spherical cut is selected, set the cut radius
- <kbd>Set cut size (cubical cut)</kbd>: If cubical cut is selected, set the cut size
- <kbd>Visualize cut geometry</kbd>: Draw an object for visualizing how and where the cut is being made

# Feature Showcasing

TODO: Insert GIFs

# Screenshots

![Head](assets/app_preview_1.png)

![Head with low opacity, skeleton fully opaque](assets/app_preview_3.png)

![Skeleton](assets/app_preview_2.png)
