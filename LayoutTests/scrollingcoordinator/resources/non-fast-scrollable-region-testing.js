// setMockScrollbarsEnabled doesn't actually invalidate scrollbars
// so if we don't set it immediately, they won't repaint/relayout
// correctly!  http://crbug.com/365509
if (window.internals)
    window.internals.settings.setMockScrollbarsEnabled(true);

// Draws green overlays for non-fast scrollable regions. This provides a visual
// feedback that is useful when running the test interactively.
function drawNonFastScrollableRegionOverlays() {
    var overlay = document.createElement("div");
    overlay.style.position = "absolute";
    overlay.style.left = 0;
    overlay.style.top = 0;
    overlay.style.opacity = 0.5;

    var rects = window.internals.nonFastScrollableRects(document);
    for (var i = 0; i < rects.length; i++) {
        var rect = rects[i];
        var patch = document.createElement("div");
        patch.style.position = "absolute";
        patch.style.backgroundColor = "#00ff00";
        patch.style.left = rect.left + "px";
        patch.style.top = rect.top  + "px";
        patch.style.width = rect.width + "px";
        patch.style.height = rect.height + "px";

        overlay.appendChild(patch);
    }

    document.body.appendChild(overlay);
}

// Waits for one RAF call to ensure compositing update has occurred and invokes task.
function awaitCompsitingUpdate(task) {
    window.requestAnimationFrame(task);
}

function rectToString(rect) {
    return '[' + [rect.left, rect.top, rect.width, rect.height].join(', ') + ']';
}

