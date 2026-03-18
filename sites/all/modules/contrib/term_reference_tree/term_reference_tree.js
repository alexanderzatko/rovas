(function($) {

if (Drupal.ajax) {
  Drupal.ajax.prototype.commands.term_reference_tree_append_children = function (ajax, response, status) {
    $(ajax.wrapper).siblings('.term-reference-tree-button').removeClass('term-reference-tree-collapsed');
    $newElement = $('<div></div>').html(response.data);
    Drupal.attachBehaviors($newElement);
    $newElement = $newElement.contents();
    $newElement.show();
    $(ajax.wrapper).replaceWith($newElement);
    var $tree = $newElement.parents('.term-reference-tree');
    checkMaxChoices($tree, false);
  }
}

/**
 * Attaches the tree behavior to the term widget form.
 */
Drupal.behaviors.termReferenceTree = {
  attach: function(context, settings) {
    // Adds custom loading class in expanding button.
    $('.term-reference-tree-widget-level-ajax-button.ajax-processed', context).once(function() {
      var $element = $(this).closest('ul.term-reference-tree-level li');
      var id = $(this).attr('id');

      Drupal.ajax[id].options.beforeSend = function (xmlhttprequest, options) {
        Drupal.ajax[id].ajaxing = true;
        $element.addClass('loading');
        return Drupal.ajax[id].beforeSend(xmlhttprequest, options);
      };

      Drupal.ajax[id].options.complete = function (xmlhttprequest, status) {
        Drupal.ajax[id].ajaxing = false;
        $element.removeClass('loading');
        if (status == 'error' || status == 'parsererror') {
          return Drupal.ajax[id].error(xmlhttprequest, ajax.url);
        }
      };
    });

    // Bind the term expand/contract button to slide toggle the list underneath.
    $('.term-reference-tree-button', context).once('term-reference-tree-button').click(function(e) {
      var $this = $(this);
      if ($this.siblings('ul').length > 0) {
        $this.toggleClass('term-reference-tree-collapsed');
        $this.siblings('ul').slideToggle('fast');
      }
      else {
        $this.siblings('div.term-reference-tree-widget-level-ajax').find(':input').trigger('mousedown');
      }
    });

    // Search filtering.
    $('.term-reference-tree-filter', context).once('term-reference-tree-filter').bind('keyup click', function() {
      var searchTerms = $(this).val().toLowerCase();
      var tree = $(this).closest('.term-reference-tree').children('ul');
      // Remove term-reference-tree-filter-match class from all label tags.
      $(tree).find('label').removeClass('term-reference-tree-filter-match');
      if (searchTerms.length > 2) {
        $(tree).find('li').each(function() {
          var hasMatch = $(this).text().toLowerCase().indexOf(searchTerms) >= 0;
          var button = $(this).children('.term-reference-tree-button');
          if (hasMatch) {
            $(button).removeClass('term-reference-tree-collapsed');
            $(button).siblings('ul').slideDown('fast');
          }
          else {
            if (!$(this).find('input:checkbox:checked').length) {
              $(button).addClass('term-reference-tree-collapsed');
              $(button).siblings('ul').slideUp('fast');
            }
          }
        });

        // Highlight labels matching the search.
        $(tree).find('label').each(function () {
          var hasMatch = $(this).text().toLowerCase().indexOf(searchTerms) >= 0;
          if (hasMatch) {
            $(this).addClass('term-reference-tree-filter-match');
          }
        });
      }
      else {
        var unselected = $(tree).find('li').filter(function() {
          return !$(this).find('input:checkbox:checked').length;
        });
        $(unselected).each(function() {
          var button = $(this).children('.term-reference-tree-button');
          $(button).addClass('term-reference-tree-collapsed');
          $(button).siblings('ul').slideUp('fast');
          $(this).removeClass('term-reference-tree-filter-match');
        });
      }
    });

    // An expand all button (unimplemented)
    /*
    $('.expandbutton').click(function() {
      $(this).siblings('.term-reference-tree-button').trigger('click');
    });
    */

    $('.term-reference-tree-level :input', context).filter('[type=radio], [type=checkbox]').once().bind('change', function(e) {
      var $this = $(this);
      var $tree = $this.parents('.term-reference-tree');
      var input_type = $this.is(':checkbox') > 0 ? 'checkbox' : 'radio';

      // Check for max choices.
      checkMaxChoices($tree, $this);

      // Select parent automatically.
      if ($this.hasClass('select-parents') && $(this).is(':checked')) {
        $this.parents('ul.term-reference-tree-level li').children('div.form-item').find(':input:not(:checked)').each(function() {
          $(this).attr('checked', 'checked').trigger('change');
        });
      }

      // Cascading selection.
      if ($tree.hasClass('term-reference-tree-cascading-selection')) {
        var $children = $this.closest('ul.term-reference-tree-level li').find(':input[id^="' + $this.attr('id') + '-children"]');
        if ($this.is(':checked')) {
          $children.filter(':not(:checked)').attr('checked', 'checked').trigger('change');
        }
        else {
          $children.filter(':checked').removeAttr('checked').trigger('change');
        }
      }

      // Change track list when controls are clicked.
      if ($tree.hasClass('term-reference-tree-track-list-shown')) {
        var track_list_container = $tree.find('.term-reference-tree-track-list');

        // Remove the "nothing selected" message if showing - add it later if needed.
        removeNothingSelectedMessage(track_list_container);
        if ($this.is(':checked') ) {
          // Control checked - add item to the track list.
          label_element = $this.closest('.form-item').find('label');
          addItemToTrackList(
            track_list_container,         // Where to add new item.
            label_element.html(),         // Text of new item.
            $(label_element).attr('for'), // Id of control new item is for.
            input_type                    // checkbox or radio.
          );
        }
        else {
          // Checkbox unchecked. Remove from the track list.
          $('#' + $this.attr('id') + '_list').remove();
        }

        // Show "nothing selected" message, if needed.
        showNothingSelectedMessage(track_list_container);
      }
    });

    $('.term-reference-tree', context).once('term-reference-tree', function() {
      var tree = $(this);

      // On page load, check whether the maximum number of choices is already selected.
      // If so, disable the other options.
      checkMaxChoices(tree, false);

      // On page load, check if the user wants a track list. If so, add the
      // currently selected items to it.
      if ($(this).hasClass('term-reference-tree-track-list-shown')) {
        var track_list_container = $(this).find('.term-reference-tree-track-list');

        // Var to track whether using checkboxes or radio buttons.
        var input_type = $(this).has('input[type=checkbox]').size() > 0 ? 'checkbox' : 'radio';

        // Find all the checked controls.
        var checked_controls = $(this).find('input[type=' + input_type + ']:checked');

        // Get their labels.
        var labels = checked_controls.parents('.form-type-checkbox, .form-type-radio').find('label');
        var label_element;

        //For each label of the checked boxes, add item to the track list.
        labels.each(function(index) {
          label_element = $(labels[index]);
          addItemToTrackList(
            track_list_container,         //Where to add new item.
            label_element.html(),         //Text of new item.
            $(label_element).attr('for'), //Id of control new item is for.
            input_type                    //checkbox or radio
          );
        }); //End labels.each

        //Show "nothing selected" message, if needed.
        showNothingSelectedMessage(track_list_container);

        //Event - when an element on the track list is clicked on:
        //  1. Delete it.
        //  2. Uncheck the associated checkbox.
        //The event is bound to the track list container, not each element.
        $(track_list_container).click(function(event){
          //Remove the "nothing selected" message if showing - add it later if needed.
          //removeNothingSelectedMessage(track_list_container);
          var event_target = $(event.target);
          var control_id = event_target.data('control_id');

          if(control_id) {
            event_target.remove();

            var checkbox = $('#' + control_id);
            checkbox.removeAttr('checked').trigger('change');
            checkMaxChoices(tree, checkbox);

            //Show "nothing selected" message, if needed.
            showNothingSelectedMessage(track_list_container);
          }
        });
      } //End Want a track list.
    });
  }
};

/**
 * Add a new item to the track list.
 * If more than one item can be selected, the new item is positioned to
 * match the order of the terms in the checkbox tree.
 *
 * @param track_list_container Container where the new item will be added.
 *
 * @param item_text Text of the item to add.
 *
 * @param control_id Id of the checkbox/radio control the item matches.
 *
 * @param control_type Control type - 'checkbox' or 'radio'.
 */
function addItemToTrackList(track_list_container, item_text, control_id, control_type) {
  var new_item = $('<li class="track-item">' + item_text + '</li>');
  new_item.data('control_id', control_id);

  //Add an id for easy finding of the item.
  new_item.attr('id', control_id + '_list');

  //Process radio controls - only one item can be selected.
  if ( control_type == 'radio') {
    //Find the existing element on the track list, if there is one.
    var current_items = track_list_container.find('li');

    //If there are no items on the track list, add the new item.
    if ( current_items.size() == 0 ) {
      track_list_container.append(new_item);
    }
    else {
      //There is an item on the list.
      var current_item = $(current_items.get(0));

      //Is the item we want to add different from what is there?
      if ( current_item.data('control_id') != control_id ) {
        //Remove exiting element from track list, and add the new one.
        current_item.remove();
        track_list_container.append(new_item);
      }
    }
    return;
  }

  //Using checkboxes, so there can be more than one selected item.
  //Find the right place to put the new item, to match the order of the
  //checkboxes.
  var list_items = track_list_container.find('li');
  var item_comparing_to;

  //Flag to tell whether the item was inserted.
  var inserted_flag = false;
  list_items.each(function(index){
    item_comparing_to = $(list_items[index]);

    //If item is already on the track list, do nothing.
    if ( control_id == item_comparing_to.data('control_id') ) {
      inserted_flag = true;
      return false; //Returning false stops the loop.
    }
    else if ( control_id < item_comparing_to.data('control_id') ) {
      //Add it here.
      item_comparing_to.before(new_item);
      inserted_flag = true;
      return false; //Returning false stops the loop.
    }
  });

  //If not inserted yet, add new item at the end of the track list.
  if ( ! inserted_flag ) {
    track_list_container.append(new_item);
  }
}

/**
 * Show the 'nothing selected' message if it applies.
 *
 * @param track_list_container Where the message is to be shown.
 */
function showNothingSelectedMessage(track_list_container) {
  //Is the message there already?
  var message_showing =
      (track_list_container.find('.term_ref_tree_nothing_message').size() != 0);

  //Number of real items showing.
  var num_real_items_showing =
      message_showing
      ? track_list_container.find('li').size() - 1
      : track_list_container.find('li').size();
  if ( num_real_items_showing == 0 ) {
    //No items showing, so show the message.
    if ( ! message_showing ) {
      track_list_container.append(
          '<li class="term_ref_tree_nothing_message">' + termReferenceTreeNothingSelectedText + '</li>'
      );
    }
  }
  else { // !(num_real_items_showing == 0)
    //There are real items.
    if ( message_showing ) {
      track_list_container.find('.term_ref_tree_nothing_message').remove();
    }
  }
}

/**
 * Remove the 'nothing selected' message. Makes processing easier.
 *
 * @param track_list_container Where the message is shown.
 */
function removeNothingSelectedMessage(track_list_container) {
  track_list_container.find('.term_ref_tree_nothing_message').remove();
}

// This helper function checks if the maximum number of choices is already selected.
// If so, it disables all the other options.  If not, it enables them.
function checkMaxChoices(item, checkbox) {
  var maxChoices = -1;
  try {
    maxChoices = parseInt(Drupal.settings.term_reference_tree.trees[item.attr('id')]['max_choices']);
  }
  catch (e){}
  var count = item.find(':checked').length;

  if(maxChoices > 0 && count >= maxChoices) {
    item.find('input[type=checkbox]:not(:checked)').attr('disabled', 'disabled').parent().addClass('disabled');
  } else if (maxChoices > 0) {
    item.find('input[type=checkbox]').removeAttr('disabled').parent().removeClass('disabled');
  }
}

})(jQuery);
