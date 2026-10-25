/**
 * Client-side blackout paint for work-report proof photos.
 */
(function ($) {
  'use strict';

  var NOTE_ID = 'rovas-photo-paint-note';
  var BTN_CLASS = 'rovas-photo-paint-btn';
  var WARN_SHOWN_KEY = 'rovasPhotoPaintWarnShown';

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
      .text(t('noteHint', 'Use “Black out areas” under each photo to cover names, emails, or phones with a black marker.'));
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
      if ($w.find('.' + BTN_CLASS).length) {
        return;
      }
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
        text: t('btnHint', 'Opens a marker tool to black out sensitive parts of this image.')
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
    var brush = parseInt(t('defaultBrush', 24), 10) || 24;

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

    var $undo = $('<button/>', { type: 'button', 'class': 'button', text: t('undoLabel', 'Undo') });
    var $clear = $('<button/>', { type: 'button', 'class': 'button', text: t('clearLabel', 'Clear marks') });
    var $apply = $('<button/>', { type: 'button', 'class': 'button button-primary', text: t('applyLabel', 'Apply') });
    var $cancel = $('<button/>', { type: 'button', 'class': 'button', text: t('cancelLabel', 'Cancel') });
    var $status = $('<span/>', { 'class': 'rovas-photo-paint-status' });

    toolbar.append($size, $undo, $clear, $apply, $cancel, $status);
    canvasWrap.append(canvas);
    panel.append(toolbar, canvasWrap);
    overlay.append(panel);
    $('body').append(overlay);

    $status.text(t('loadingLabel', 'Loading full image…'));
    $apply.prop('disabled', true);
    $undo.prop('disabled', true);
    $clear.prop('disabled', true);

    function pushUndo() {
      try {
        undoStack.push(ctx.getImageData(0, 0, canvas.width, canvas.height));
        if (undoStack.length > 25) {
          undoStack.shift();
        }
      }
      catch (err) {
        // Ignore quota errors on huge canvases.
      }
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
      lastX = p.x;
      lastY = p.y;
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
    }

    $(canvas).on('mousedown', onDown);
    $(canvas).on('mousemove', onMove);
    $(window).on('mouseup.rovasPaint', onUp);
    $(canvas).on('touchstart', onDown);
    $(canvas).on('touchmove', onMove);
    $(canvas).on('touchend touchcancel', onUp);

    $undo.on('click', function (e) {
      e.preventDefault();
      if (!undoStack.length) {
        return;
      }
      ctx.putImageData(undoStack.pop(), 0, 0);
    });

    $clear.on('click', function (e) {
      e.preventDefault();
      if (!undoStack.length) {
        return;
      }
      ctx.putImageData(undoStack[0], 0, 0);
      undoStack = [];
    });

    $cancel.on('click', function (e) {
      e.preventDefault();
      close();
    });

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
          // Refresh the form thumbnail (image style), not the full original URL.
          var base = (thumbSrc || '').split('?')[0];
          if (base) {
            var bust = base + '?paint=' + Date.now();
            $img.attr('src', bust);
            $widget.find('.image-preview img').attr('src', bust);
          }
          close();
        }).fail(function () {
          $status.text(t('errorLabel', 'Could not save.'));
          $apply.prop('disabled', false);
        });
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
        var maxW = Math.min(1200, window.innerWidth - 48);
        var maxH = Math.min(700, window.innerHeight - 140);
        var w = image.naturalWidth || image.width;
        var h = image.naturalHeight || image.height;
        var scale = Math.min(1, maxW / w, maxH / h);
        // Paint at native (original file) resolution; CSS scales the display.
        canvas.width = w;
        canvas.height = h;
        canvas.style.width = Math.round(w * scale) + 'px';
        canvas.style.height = Math.round(h * scale) + 'px';
        ctx.drawImage(image, 0, 0);
        undoStack = [];
        pushUndo();
        $status.text('');
        $apply.prop('disabled', false);
        $undo.prop('disabled', false);
        $clear.prop('disabled', false);
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
    updateNote($root);
    ensureButtons($root);
  }

  Drupal.behaviors.rovasPhotoPaint = {
    attach: function () {
      refresh();
      $(document).off('ajaxComplete.rovasPhotoPaint').on('ajaxComplete.rovasPhotoPaint', function () {
        refresh();
      });
    }
  };
})(jQuery);
