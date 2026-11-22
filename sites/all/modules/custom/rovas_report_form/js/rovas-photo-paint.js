/**
 * Client-side rotate + blackout paint for work-report proof photos.
 *
 * Apply stages the edited JPEG through the field's normal Upload button
 * (Remove → assign File → Upload) so form submit and thumbnails stay in sync.
 */
(function ($) {
  'use strict';

  var NOTE_ID = 'rovas-photo-paint-note';
  var BTN_CLASS = 'rovas-photo-paint-btn';
  var THUMB_CLASS = 'rovas-photo-paint-thumb';
  var WARN_SHOWN_KEY = 'rovasPhotoPaintWarnShown';
  // fid input name → File waiting to be assigned after Remove rebuilds the widget.
  var pendingPaintFiles = {};
  var pendingPaintUploading = {};

  function settings() {
    return (Drupal.settings && Drupal.settings.rovasPhotoPaint) || {};
  }

  function t(key, fallback) {
    var s = settings();
    return s[key] || fallback;
  }

  function fieldRoot() {
    return $('.field-name-field-photos').first();
  }

  function widgetsWithFid($root) {
    return $root.find('.image-widget, .form-managed-file').filter(function () {
      var $w = $(this);
      var fid = $w.find('input[name*="[fid]"]').filter(function () {
        return parseInt(this.value, 10) > 0;
      }).first().val();
      return !!fid;
    });
  }

  function findFidInput($root, fidName) {
    return $root.find('input').filter(function () {
      return this.name === fidName;
    }).first();
  }

  function blobToPaintFile(blob) {
    var name = 'proof_painted.jpg';
    var type = 'image/jpeg';
    try {
      return new File([blob], name, { type: type });
    }
    catch (err) {
      try {
        blob.name = name;
        blob.lastModifiedDate = new Date();
      }
      catch (err2) {
        // Ignore.
      }
      return blob;
    }
  }

  /**
   * After Remove AJAX, put the painted file into the file input and click Upload.
   */
  function flushPendingPaint($root) {
    $.each(pendingPaintFiles, function (fidName, file) {
      if (pendingPaintUploading[fidName]) {
        return;
      }
      var $fid = findFidInput($root, fidName);
      if (!$fid.length) {
        return;
      }
      var fid = parseInt($fid.val(), 10) || 0;
      if (fid > 0) {
        // Upload already finished (or another path restored a file).
        delete pendingPaintFiles[fidName];
        delete pendingPaintUploading[fidName];
        return;
      }
      var $w = $fid.closest('.image-widget, .form-managed-file');
      var $input = $w.find('input[type="file"]').first();
      var $upload = $w.find('input[name*="_upload_button"]').first();
      if (!$input.length || !$upload.length || typeof DataTransfer === 'undefined') {
        delete pendingPaintFiles[fidName];
        return;
      }
      try {
        var dt = new DataTransfer();
        dt.items.add(file);
        $input[0].files = dt.files;
      }
      catch (err) {
        delete pendingPaintFiles[fidName];
        return;
      }
      pendingPaintUploading[fidName] = true;
      delete pendingPaintFiles[fidName];
      // Drupal 7 file AJAX listens on mousedown.
      $upload.trigger('mousedown');
    });
  }

  /**
   * Stage painted blob via managed-file Remove → Upload. Falls back to false.
   */
  function stagePaintViaUpload(blob, $widget) {
    if (typeof DataTransfer === 'undefined') {
      return false;
    }
    var $fid = $widget.find('input[name*="[fid]"]').filter(function () {
      return parseInt(this.value, 10) > 0;
    }).first();
    if (!$fid.length) {
      return false;
    }
    var fidName = $fid.attr('name');
    var $remove = $widget.find('input[name*="_remove_button"]').first();
    if (!fidName || !$remove.length) {
      return false;
    }
    var file = blobToPaintFile(blob);
    pendingPaintFiles[fidName] = file;
    delete pendingPaintUploading[fidName];
    $remove.trigger('mousedown');
    return true;
  }

  function showPrivacyModal() {
    if ($('.rovas-photo-paint-warn-overlay').length) {
      return;
    }
    var $overlay = $('<div/>', {
      'class': 'rovas-photo-paint-warn-overlay',
      role: 'alertdialog',
      'aria-modal': 'true',
      'aria-labelledby': 'rovas-photo-paint-warn-title'
    });
    var $panel = $('<div/>', { 'class': 'rovas-photo-paint-warn-panel' });
    var $title = $('<h2/>', {
      id: 'rovas-photo-paint-warn-title',
      text: t('noteTitle', 'Public proof photos')
    });
    var $body = $('<p/>').text(t('note', ''));
    var $hint = $('<p/>', { 'class': 'rovas-photo-paint-warn-hint' })
      .text(t('noteHint', 'Use “Black out areas” under each photo (or click the thumbnail) to rotate the image or cover names, emails, or phones with a black marker.'));
    var $ok = $('<button/>', {
      type: 'button',
      'class': 'button button-primary',
      text: t('noteOk', 'Got it')
    });
    function close() {
      $overlay.remove();
    }
    $ok.on('click', function (e) {
      e.preventDefault();
      close();
    });
    $overlay.on('click', function (e) {
      if (e.target === $overlay[0]) {
        close();
      }
    });
    $(document).on('keydown.rovasPhotoWarn', function (e) {
      if (e.keyCode === 27) {
        $(document).off('keydown.rovasPhotoWarn');
        close();
      }
    });
    $panel.append($title, $body, $hint, $ok);
    $overlay.append($panel);
    $('body').append($overlay);
    $ok.focus();
  }

  function updateNote($root) {
    var $widgets = widgetsWithFid($root);
    var $note = $('#' + NOTE_ID);
    if (!$widgets.length) {
      // Keep note while a painted re-upload is in flight (fid briefly cleared).
      if (!$.isEmptyObject(pendingPaintFiles) || !$.isEmptyObject(pendingPaintUploading)) {
        return;
      }
      $note.hide();
      try {
        sessionStorage.removeItem(WARN_SHOWN_KEY);
      }
      catch (err) {
        // Ignore.
      }
      return;
    }
    if (!$note.length) {
      $note = $('<div/>', {
        id: NOTE_ID,
        'class': 'rovas-photo-paint-note messages warning',
        role: 'status'
      }).html(
        '<strong>' + $('<div/>').text(t('noteTitle', 'Public proof photos')).html() + '</strong> '
        + $('<div/>').text(t('note', '')).html()
        + ' <em>' + $('<div/>').text(t('noteHint', '')).html() + '</em>'
      );
      $root.prepend($note);
    }
    $note.show();

    var shown = false;
    try {
      shown = sessionStorage.getItem(WARN_SHOWN_KEY) === '1';
    }
    catch (err2) {
      shown = false;
    }
    if (!shown) {
      try {
        sessionStorage.setItem(WARN_SHOWN_KEY, '1');
      }
      catch (err3) {
        // Ignore.
      }
      showPrivacyModal();
    }
  }

  function ensureButtons($root) {
    widgetsWithFid($root).each(function () {
      var $w = $(this);
      var $fid = $w.find('input[name*="[fid]"]').filter(function () {
        return parseInt(this.value, 10) > 0;
      }).first();
      if (!$fid.length) {
        return;
      }
      var fid = parseInt($fid.val(), 10);
      var $img = $w.find('.image-preview img, img').first();
      if (!$img.length) {
        return;
      }

      // Thumbnail opens the editor.
      if (!$img.hasClass(THUMB_CLASS)) {
        $img.addClass(THUMB_CLASS);
        $img.attr('title', t('thumbTitle', 'Click to rotate or black out sensitive areas'));
        $img.off('click.rovasPhotoPaint').on('click.rovasPhotoPaint', function (e) {
          e.preventDefault();
          openEditor(fid, $img, $w);
        });
      }

      if ($w.find('.' + BTN_CLASS).length) {
        return;
      }
      var $btn = $('<button/>', {
        type: 'button',
        'class': 'button button-primary ' + BTN_CLASS,
        text: t('btnLabel', 'Black out areas')
      });
      $btn.on('click', function (e) {
        e.preventDefault();
        openEditor(fid, $img, $w);
      });
      var $wrap = $('<div/>', { 'class': 'rovas-photo-paint-actions' });
      $wrap.append($btn);
      $wrap.append($('<div/>', {
        'class': 'rovas-photo-paint-btn-hint description',
        text: t('btnHint', 'Opens a tool to rotate the photo or black out sensitive parts. You can also click the thumbnail.')
      }));
      var $slot = $w.find('.image-widget-data').first();
      if ($slot.length) {
        $slot.append($wrap);
      }
      else {
        $w.append($wrap);
      }
    });
  }

  function openEditor(fid, $img, $widget) {
    var thumbSrc = $img.attr('src') || '';

    var overlay = $('<div/>', { 'class': 'rovas-photo-paint-overlay', role: 'dialog', 'aria-modal': 'true' });
    var panel = $('<div/>', { 'class': 'rovas-photo-paint-panel' });
    var toolbar = $('<div/>', { 'class': 'rovas-photo-paint-toolbar' });
    var canvasWrap = $('<div/>', { 'class': 'rovas-photo-paint-canvas-wrap' });
    var canvas = document.createElement('canvas');
    var ctx = canvas.getContext('2d');
    var undoStack = [];
    var drawing = false;
    var lastX = 0;
    var lastY = 0;
    var startX = 0;
    var startY = 0;
    var brush = parseInt(t('defaultBrush', 24), 10) || 24;
    var straighten = false;
    var ready = false;

    var $size = $('<label/>').append(
      document.createTextNode(t('brushLabel', 'Marker size') + ' '),
      $('<input/>', {
        type: 'range',
        min: 4,
        max: 80,
        value: brush,
        'class': 'rovas-photo-paint-size'
      })
    );
    $size.find('input').on('input change', function () {
      brush = parseInt(this.value, 10) || 24;
    });

    var $straight = $('<label/>', { 'class': 'rovas-photo-paint-straight' }).append(
      $('<input/>', {
        type: 'checkbox',
        'class': 'rovas-photo-paint-straight-cb'
      }),
      document.createTextNode(' ' + t('straightLabel', 'Make lines straight'))
    );
    $straight.find('input').on('change', function () {
      straighten = !!this.checked;
    });

    var $rotateLeft = $('<button/>', {
      type: 'button',
      'class': 'button rovas-photo-paint-rotate',
      text: t('rotateLeftLabel', 'Rotate left'),
      title: t('rotateLeftLabel', 'Rotate left')
    });
    var $rotateRight = $('<button/>', {
      type: 'button',
      'class': 'button rovas-photo-paint-rotate',
      text: t('rotateRightLabel', 'Rotate right'),
      title: t('rotateRightLabel', 'Rotate right')
    });
    var $undo = $('<button/>', { type: 'button', 'class': 'button', text: t('undoLabel', 'Back') });
    var $clear = $('<button/>', { type: 'button', 'class': 'button', text: t('clearLabel', 'Clear marks') });
    var $apply = $('<button/>', { type: 'button', 'class': 'button button-primary', text: t('applyLabel', 'Apply') });
    var $cancel = $('<button/>', { type: 'button', 'class': 'button', text: t('cancelLabel', 'Cancel') });
    var $status = $('<span/>', { 'class': 'rovas-photo-paint-status' });

    toolbar.append($rotateLeft, $rotateRight, $size, $straight, $undo, $clear, $apply, $cancel, $status);
    canvasWrap.append(canvas);
    panel.append(toolbar, canvasWrap);
    overlay.append(panel);
    $('body').append(overlay);

    $status.text(t('loadingLabel', 'Loading full image…'));
    $apply.prop('disabled', true);
    $undo.prop('disabled', true);
    $clear.prop('disabled', true);
    $rotateLeft.prop('disabled', true);
    $rotateRight.prop('disabled', true);

    function fitDisplaySize() {
      var maxW = Math.min(1200, window.innerWidth - 48);
      var maxH = Math.min(700, window.innerHeight - 140);
      var w = canvas.width;
      var h = canvas.height;
      if (!w || !h) {
        return;
      }
      var scale = Math.min(1, maxW / w, maxH / h);
      canvas.style.width = Math.round(w * scale) + 'px';
      canvas.style.height = Math.round(h * scale) + 'px';
    }

    /**
     * Snapshot current pixels (and dimensions) so rotate can be undone.
     */
    function snapshotCanvas() {
      var snap = document.createElement('canvas');
      snap.width = canvas.width;
      snap.height = canvas.height;
      snap.getContext('2d').drawImage(canvas, 0, 0);
      return snap;
    }

    function restoreSnapshot(snap) {
      canvas.width = snap.width;
      canvas.height = snap.height;
      fitDisplaySize();
      ctx.drawImage(snap, 0, 0);
    }

    function updateUndoButtons() {
      // Keep index 0 as the clean baseline; Back only when a stroke/rotate can be undone.
      var canUndo = undoStack.length > 1;
      $undo.prop('disabled', !canUndo);
      $clear.prop('disabled', !canUndo);
    }

    function pushUndo() {
      try {
        undoStack.push(snapshotCanvas());
        if (undoStack.length > 25) {
          // Drop oldest stroke snapshot, keep baseline at [0].
          undoStack.splice(1, 1);
        }
        updateUndoButtons();
      }
      catch (err) {
        // Ignore quota errors on huge canvases.
      }
    }

    /**
     * Rotate 90° clockwise (dir > 0) or counter-clockwise (dir < 0).
     */
    function rotateCanvas(dir) {
      if (!ready || !canvas.width || !canvas.height) {
        return;
      }
      pushUndo();
      var w = canvas.width;
      var h = canvas.height;
      var tmp = document.createElement('canvas');
      tmp.width = h;
      tmp.height = w;
      var tctx = tmp.getContext('2d');
      if (dir > 0) {
        tctx.translate(h, 0);
        tctx.rotate(Math.PI / 2);
      }
      else {
        tctx.translate(0, w);
        tctx.rotate(-Math.PI / 2);
      }
      tctx.drawImage(canvas, 0, 0);
      canvas.width = tmp.width;
      canvas.height = tmp.height;
      fitDisplaySize();
      ctx.drawImage(tmp, 0, 0);
    }

    function canvasPos(evt) {
      var rect = canvas.getBoundingClientRect();
      var clientX = evt.clientX;
      var clientY = evt.clientY;
      if (evt.touches && evt.touches[0]) {
        clientX = evt.touches[0].clientX;
        clientY = evt.touches[0].clientY;
      }
      var scaleX = canvas.width / rect.width;
      var scaleY = canvas.height / rect.height;
      return {
        x: (clientX - rect.left) * scaleX,
        y: (clientY - rect.top) * scaleY
      };
    }

    function strokeTo(x, y) {
      ctx.strokeStyle = '#000';
      ctx.fillStyle = '#000';
      ctx.lineWidth = brush;
      ctx.lineCap = 'round';
      ctx.lineJoin = 'round';
      ctx.beginPath();
      ctx.moveTo(lastX, lastY);
      ctx.lineTo(x, y);
      ctx.stroke();
      lastX = x;
      lastY = y;
    }

    function onDown(evt) {
      evt.preventDefault();
      pushUndo();
      drawing = true;
      var p = canvasPos(evt.originalEvent || evt);
      startX = lastX = p.x;
      startY = lastY = p.y;
      ctx.beginPath();
      ctx.fillStyle = '#000';
      ctx.arc(lastX, lastY, brush / 2, 0, Math.PI * 2);
      ctx.fill();
    }

    function onMove(evt) {
      if (!drawing) {
        return;
      }
      evt.preventDefault();
      var p = canvasPos(evt.originalEvent || evt);
      strokeTo(p.x, p.y);
    }

    function onUp(evt) {
      if (!drawing) {
        return;
      }
      evt.preventDefault();
      drawing = false;
      if (!straighten || !undoStack.length) {
        return;
      }
      // Replace freehand stroke with a straight segment from first to last point.
      var endX = lastX;
      var endY = lastY;
      try {
        restoreSnapshot(undoStack[undoStack.length - 1]);
      }
      catch (err) {
        return;
      }
      lastX = startX;
      lastY = startY;
      ctx.beginPath();
      ctx.fillStyle = '#000';
      ctx.arc(startX, startY, brush / 2, 0, Math.PI * 2);
      ctx.fill();
      strokeTo(endX, endY);
    }

    $(canvas).on('mousedown', onDown);
    $(canvas).on('mousemove', onMove);
    $(window).on('mouseup.rovasPaint', onUp);
    $(canvas).on('touchstart', onDown);
    $(canvas).on('touchmove', onMove);
    $(canvas).on('touchend touchcancel', onUp);

    $rotateLeft.on('click', function (e) {
      e.preventDefault();
      rotateCanvas(-1);
    });

    $rotateRight.on('click', function (e) {
      e.preventDefault();
      rotateCanvas(1);
    });

    $undo.on('click', function (e) {
      e.preventDefault();
      if (undoStack.length <= 1) {
        return;
      }
      restoreSnapshot(undoStack.pop());
      updateUndoButtons();
    });

    $clear.on('click', function (e) {
      e.preventDefault();
      if (undoStack.length <= 1) {
        return;
      }
      restoreSnapshot(undoStack[0]);
      undoStack = [undoStack[0]];
      updateUndoButtons();
    });

    $cancel.on('click', function (e) {
      e.preventDefault();
      close();
    });

    function applyViaAjaxFallback(blob) {
      var fd = new FormData();
      fd.append('fid', String(fid));
      fd.append('token', settings().token || '');
      fd.append('image', blob, 'proof_painted.jpg');
      $.ajax({
        url: settings().saveUrl,
        type: 'POST',
        data: fd,
        processData: false,
        contentType: false,
        dataType: 'json'
      }).done(function (res) {
        if (!res || !res.ok) {
          $status.text((res && res.error) ? res.error : t('errorLabel', 'Could not save.'));
          $apply.prop('disabled', false);
          return;
        }
        var nextSrc = res.thumbUrl || '';
        if (!nextSrc && thumbSrc) {
          // Keep itok / existing query; only add a cache buster.
          nextSrc = thumbSrc + (thumbSrc.indexOf('?') >= 0 ? '&' : '?') + 'paint=' + Date.now();
        }
        if (nextSrc) {
          $img.attr('src', nextSrc);
          $widget.find('.image-preview img').attr('src', nextSrc);
        }
        close();
      }).fail(function () {
        $status.text(t('errorLabel', 'Could not save.'));
        $apply.prop('disabled', false);
      });
    }

    $apply.on('click', function (e) {
      e.preventDefault();
      $status.text(t('savingLabel', 'Saving…'));
      $apply.prop('disabled', true);
      if (!canvas.toBlob) {
        $status.text(t('errorLabel', 'Could not save. Try another browser.'));
        $apply.prop('disabled', false);
        return;
      }
      canvas.toBlob(function (blob) {
        if (!blob) {
          $status.text(t('errorLabel', 'Could not save. Try another browser.'));
          $apply.prop('disabled', false);
          return;
        }
        // Prefer the field Upload path so the painted file is what the form submits.
        if (stagePaintViaUpload(blob, $widget)) {
          close();
          return;
        }
        applyViaAjaxFallback(blob);
      }, 'image/jpeg', 0.92);
    });

    function close() {
      $(window).off('.rovasPaint');
      overlay.remove();
    }

    overlay.on('click', function (e) {
      if (e.target === overlay[0]) {
        close();
      }
    });

    function loadCanvasFromUrl(fullUrl) {
      var image = new Image();
      image.crossOrigin = 'anonymous';
      image.onload = function () {
        var w = image.naturalWidth || image.width;
        var h = image.naturalHeight || image.height;
        // Paint at native (original file) resolution; CSS scales the display.
        canvas.width = w;
        canvas.height = h;
        fitDisplaySize();
        ctx.drawImage(image, 0, 0);
        undoStack = [];
        pushUndo();
        ready = true;
        $status.text('');
        $apply.prop('disabled', false);
        $rotateLeft.prop('disabled', false);
        $rotateRight.prop('disabled', false);
        updateUndoButtons();
      };
      image.onerror = function () {
        $status.text(t('loadErrorLabel', 'Could not load the full image.'));
      };
      image.src = fullUrl + (fullUrl.indexOf('?') >= 0 ? '&' : '?') + 'paintload=' + Date.now();
    }

    // Resolve original managed-file URL (not the image-style thumbnail).
    var sourceEndpoint = settings().sourceUrl;
    if (!sourceEndpoint) {
      $status.text(t('loadErrorLabel', 'Could not load the full image.'));
      return;
    }
    $.ajax({
      url: sourceEndpoint,
      type: 'GET',
      dataType: 'json',
      data: {
        fid: fid,
        token: settings().token || ''
      }
    }).done(function (res) {
      if (!res || !res.ok || !res.url) {
        $status.text(t('loadErrorLabel', 'Could not load the full image.'));
        return;
      }
      loadCanvasFromUrl(res.url);
    }).fail(function () {
      $status.text(t('loadErrorLabel', 'Could not load the full image.'));
    });
  }

  function refresh() {
    var $root = fieldRoot();
    if (!$root.length) {
      return;
    }
    flushPendingPaint($root);
    updateNote($root);
    ensureButtons($root);
  }

  Drupal.behaviors.rovasPhotoPaint = {
    attach: function () {
      refresh();
      $(document).off('ajaxComplete.rovasPhotoPaint').on('ajaxComplete.rovasPhotoPaint', function () {
        // Clear uploading flags once the widget has a new fid again.
        var $root = fieldRoot();
        if ($root.length) {
          $.each(pendingPaintUploading, function (fidName) {
            var $fid = findFidInput($root, fidName);
            if ($fid.length && parseInt($fid.val(), 10) > 0) {
              delete pendingPaintUploading[fidName];
            }
          });
        }
        refresh();
      });
    }
  };
})(jQuery);
